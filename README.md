# inject_child

Instructions to try:

1. `gcc child.c -o child`
2. `ld -r -b binary -o childbin.o child`
3. `gcc -fPIC -shared injector.c childbin.o -o injector.so`
4. `LD_PRELOAD=./injector.so /bin/echo Hello from the target binary`

## Copying

Use LD_PRELOAD injection to spawn a child process
Copyright (C) 2024  Joseph C. Sible

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
