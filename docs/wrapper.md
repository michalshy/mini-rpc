# Generyczny `wrap_function` — dekodowanie argumentów z szablonów

## Krok 1: Wyciąganie typów argumentów z funkcji (function_traits)

Potrzebujemy "trait", który z typu funkcji wyciągnie typy argumentów i typ zwracany.

```cpp
// Bazowy szablon — nigdy nie jest dopasowany, służy jako "fallback".
// Jeśli ktoś poda typ, który nie pasuje do żadnej specjalizacji poniżej,
// dostanie błąd kompilacji.
// Ref: https://en.cppreference.com/w/cpp/language/class_template
template<typename F>
struct function_traits;
```

- `template<typename F>` — deklarujemy szablon parametryzowany jednym typem `F`
- Brak ciała (`struct function_traits;`) — to celowo niekompletna definicja (forward declaration)

```cpp
// Specjalizacja częściowa dla wskaźników do metod (np. &Foo::operator())
// Ref: https://en.cppreference.com/w/cpp/language/partial_specialization
template<typename R, typename C, typename... Args>
struct function_traits<R(C::*)(Args...) const> {
    using return_type = R;
    using args_tuple  = std::tuple<std::decay_t<Args>...>;
};
```

- `R(C::*)(Args...) const` — wzorzec dopasowania do wskaźnika na **const** metodę klasy `C`,
  zwracającą `R`, z argumentami `Args...`
  - Ref: https://en.cppreference.com/w/cpp/language/pointer#Pointers_to_member_functions
- `R` — typ zwracany funkcji
- `C` — klasa, do której należy metoda (dla lambdy to jej anonimowy typ)
- `Args...` — parameter pack — zero lub więcej typów argumentów
  - Ref: https://en.cppreference.com/w/cpp/language/parameter_pack
- `std::tuple<std::decay_t<Args>...>` — pakujemy typy argumentów w `std::tuple`,
  używając `decay_t` żeby usunąć referencje i const
  - Ref: https://en.cppreference.com/w/cpp/utility/tuple
  - Ref: https://en.cppreference.com/w/cpp/types/decay

```cpp
// Specjalizacja dla lambd i funktorów — deleguje do ich operator()
template<typename F>
struct function_traits : function_traits<decltype(&F::operator())> {};
```

- `decltype(&F::operator())` — pobiera typ `operator()` z lambdy/funktora
  - Ref: https://en.cppreference.com/w/cpp/language/decltype
- Dziedziczenie z `function_traits<...>` — "przekierowuje" do specjalizacji
  dla wskaźnika do metody powyżej

```cpp
// Specjalizacja dla zwykłych wskaźników do funkcji
template<typename R, typename... Args>
struct function_traits<R(*)(Args...)> {
    using return_type = R;
    using args_tuple  = std::tuple<std::decay_t<Args>...>;
};
```

- `R(*)(Args...)` — wzorzec dla zwykłego wskaźnika na funkcję (nie metoda klasy)

---

## Krok 2: Dekodowanie argumentów z bufora

### Dekodowanie jednego argumentu

```cpp
template<typename T>
requires std::is_trivially_copyable_v<T>
T decode_arg(const std::byte* data, int& offset) {
    T value;
    std::memcpy(&value, data + offset, sizeof(T));
    offset += sizeof(T);
    return value;
}
```

- `template<typename T>` — działa dla dowolnego typu `T`
- `requires std::is_trivially_copyable_v<T>` — constraint — kompilator odmówi jeśli `T`
  nie jest trivially copyable (np. `std::string` nie przejdzie — i słusznie, bo `memcpy`
  na stringu to UB)
  - Ref: https://en.cppreference.com/w/cpp/language/constraints
  - Ref: https://en.cppreference.com/w/cpp/types/is_trivially_copyable
- `std::memcpy(&value, data + offset, sizeof(T))` — kopiuje surowe bajty z bufora do `value`
  - Ref: https://en.cppreference.com/w/cpp/string/byte/memcpy
- `offset += sizeof(T)` — przesuwa offset o rozmiar typu, żeby następny argument
  czytał z właściwego miejsca

### Dekodowanie wszystkich argumentów naraz

```cpp
template<typename Tuple, std::size_t... I>
Tuple decode_args_impl(const std::byte* data, int& offset,
                       std::index_sequence<I...>) {
    return Tuple{ decode_arg<std::tuple_element_t<I, Tuple>>(data, offset)... };
}
```

To jest serce mechanizmu. Rozbijmy to:

- `typename Tuple` — np. `std::tuple<int, float, double>`
- `std::size_t... I` — non-type parameter pack — sekwencja indeksów, np. `0, 1, 2`
  - Ref: https://en.cppreference.com/w/cpp/utility/integer_sequence
- `std::tuple_element_t<I, Tuple>` — wyciąga I-ty typ z tuple'a.
  Dla `I=0` i `Tuple=tuple<int,float>` daje `int`
  - Ref: https://en.cppreference.com/w/cpp/utility/tuple/tuple_element
- `decode_arg<...>(data, offset)...` — trzykropek (`...`) **rozwija** pack.
  Kompilator generuje:
  ```cpp
  // Dla tuple<int, float, double> i I = 0,1,2:
  Tuple{ decode_arg<int>(data, offset),
         decode_arg<float>(data, offset),
         decode_arg<double>(data, offset) }
  ```
  - Ref: https://en.cppreference.com/w/cpp/language/parameter_pack#Pack_expansion
- Kolejność ewaluacji jest gwarantowana bo używamy brace-init `Tuple{...}`
  (list initialization gwarantuje left-to-right)
  - Ref: https://en.cppreference.com/w/cpp/language/list_initialization
    — "the initializer clauses... are evaluated in the order they appear"

### Wygodny wrapper

```cpp
template<typename Tuple>
Tuple decode_all_args(const std::byte* data, int& offset) {
    return decode_args_impl<Tuple>(
        data, offset,
        std::make_index_sequence<std::tuple_size_v<Tuple>>{}
    );
}
```

- `std::tuple_size_v<Tuple>` — daje liczbę elementów w tuple
  (np. 3 dla `tuple<int,float,double>`)
  - Ref: https://en.cppreference.com/w/cpp/utility/tuple/tuple_size
- `std::make_index_sequence<3>{}` — generuje typ `std::index_sequence<0, 1, 2>` —
  tworzymy go, żeby kompilator mógł wydedukować `I...` w `decode_args_impl`
  - Ref: https://en.cppreference.com/w/cpp/utility/integer_sequence

---

## Krok 3: Nowy `wrap_function`

```cpp
template<class F>
auto wrap_function(F&& func) {
    return Handler {
        [f = std::forward<F>(func)]
        (const buffer& req, buffer& res)
        {
            // 1. Wyciągamy typy argumentów z sygnatury f
            using traits = function_traits<std::decay_t<decltype(f)>>;
            using ArgsTuple = typename traits::args_tuple;

            // 2. Offset wskazuje na początek argumentów w buforze
            int offset = 0;

            // 3. Dekodujemy wszystkie argumenty z bufora
            ArgsTuple args = decode_all_args<ArgsTuple>(req.data(), offset);

            // 4. Wywołujemy funkcję z rozpakowanym tuple
            if constexpr (std::is_void_v<typename traits::return_type>) {
                std::apply(f, args);
            } else {
                auto result = std::apply(f, args);
                encode_arg(res, result);
            }
        }
    };
}
```

- `std::decay_t<decltype(f)>` — `decltype(f)` daje typ lambdy,
  `decay_t` usuwa referencje/const — potrzebne żeby dopasować do naszego `function_traits`
- `using ArgsTuple = typename traits::args_tuple` — to np. `std::tuple<int, int>`
  jeśli `f` przyjmuje dwa inty
- `decode_all_args<ArgsTuple>(req.data(), offset)` — dekoduje argumenty z bufora `req`
  według typów z tuple'a
- `std::apply(f, args)` — wywołuje `f` z argumentami rozpakowanymi z tuple'a
  - Ref: https://en.cppreference.com/w/cpp/utility/apply
  - `std::apply(f, std::tuple{1, 2})` → `f(1, 2)`
- `if constexpr (...)` — compile-time if — kompilator wybiera jedną gałąź
  na etapie kompilacji
  - Ref: https://en.cppreference.com/w/cpp/language/if#Constexpr_if
- `std::is_void_v<...>` — sprawdza czy typ zwracany to `void`
  - Ref: https://en.cppreference.com/w/cpp/types/is_void

---

## Jak to działa end-to-end

```cpp
// Rejestrujesz:
server.register_handler("add", [](int a, int b) { return a + b; });

// Kompilator dedukuje:
//   function_traits → return_type = int, args_tuple = tuple<int, int>
//   decode_all_args → decode_arg<int>, decode_arg<int>
//   std::apply      → lambda(decoded_a, decoded_b)
//   encode_arg      → wynik int do bufora odpowiedzi
```

---

## Wymagane includy

```cpp
#include <cstddef>      // std::byte, std::size_t
#include <cstring>       // std::memcpy
#include <functional>    // std::function
#include <tuple>         // std::tuple, std::apply, std::tuple_element_t, std::tuple_size_v
#include <type_traits>   // std::decay_t, std::is_void_v, std::is_trivially_copyable_v
#include <utility>       // std::forward, std::index_sequence, std::make_index_sequence
```
