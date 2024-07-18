# TODO

- remove use of `std::variant`

  - unnecessary calls to `std::holds_alternative<T>` which cause code bloat and understanding of type
  - performance issues
  - maintenance issues

- use interface type `Object` for `StringObject`, `BoolObject`, `NullObject`, `NumberObject`, etc
  - use tag type to denote what type of object it is
