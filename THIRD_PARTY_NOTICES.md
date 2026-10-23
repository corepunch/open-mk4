# Third-party source

The animation decoder and rotation formulas in `common/mk4_assets.c` are
adapted from the reconstructed routines in [mk4decomp](https://github.com/nathan-casabieille/mk4decomp),
commit `af88007`. Its MIT notice is reproduced below. The reference checkouts
retain their own license information. Game assets and the original executable
are supplied separately by the user.

This project is a matching decompilation of Mortal Kombat 4 (PC, 1998).

The MIT License below applies to the original contributions of this
project's authors: the reconstructed C source, headers, build
infrastructure, tools, scripts, and documentation. It does NOT grant
any rights to Mortal Kombat 4 itself, its assets, or any code or data
owned by Warner Bros. Entertainment Inc., NetherRealm Studios, or
Midway Games, which remain the property of their respective owners.

The compiled output of this project is byte-identical to the original
binary; it has no independent redistribution value. Running the result
requires a legally-obtained copy of Mortal Kombat 4.

----

MIT License

Copyright (c) 2026 Nathan Casabieille

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

## Lua 5.4.7

`third_party/lua-5.4.7/` is the unmodified Lua 5.4.7 interpreter source from
https://www.lua.org/ftp/lua-5.4.7.tar.gz (SHA-256
`9fbf5e28ef86c69858f6d3d34eccc32e911c1a28b4120ff3e84aaa70cfbf1e30`) without
the standalone `lua.c`/`luac.c` front ends. It runs the fighter scripts in the
game and in every test that runs a match. Its MIT notice is in
`third_party/lua-5.4.7/LICENSE`:

> Copyright (c) 1994-2024 Lua.org, PUC-Rio.
>
> Permission is hereby granted, free of charge, to any person obtaining a copy
> of this software and associated documentation files (the "Software"), to deal
> in the Software without restriction, including without limitation the rights
> to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
> copies of the Software, and to permit persons to whom the Software is
> furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in
> all copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
> IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
> FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
> AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
> LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
> OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
> THE SOFTWARE.
