# llvm-pass-skeleton

A slightly-less-than-completely useless LLVM pass. Now it finds potentially-unsafe divisions (i.e. those guaranteed 
to not be 0 within the same basic block) and wraps them in a guarded division function that prints and returns 0 on 
division by 0. It's still for LLVM 3.8.

Build:

    $ cd llvm-pass-skeleton
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ cd ..

Run:

    $ clang -Xclang -load -Xclang build/skeleton/libSkeletonPass.* something.c
