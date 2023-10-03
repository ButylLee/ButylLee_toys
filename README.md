# ButylLee_toys

* [VarTypeDict](VarTypeDict): A Dictionary that can contain various type value, using C++ template meta-programming.
* [DynArray](DynArray): A multi-dimension Dynamic Array that acts like VLA in C, but alloc in heap, using proxy class and template meta-programming, has the performance approaching to the built-in array.
* [EncryptedString](EncryptedString): A convenient way to create encrypted strings in source code, which makes string literals invisible in binary executable after compiling and decrypts it in run-time. The implementation requires C++20 and the resultant behavior depends on the compiler.
* [Pythonic](Pythonic): Some facilities that provide pythonic ways to write C++ codes. [**range**](Pythonic/range.h) provides an easy way to iterate an integer sequence as in Python. [**enumerate**](Pythonic/enumerate.h) allows you to iterate both indices and values in range-based loop(now you can use **enumerate** in range-v3 or C++23).
