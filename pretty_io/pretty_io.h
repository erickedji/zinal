/*
 * ZINAL (Zinal Is Not A Library) - A collection of a few programs
 * written by a dwarf standing on the shoulders of giants.
 *
 * Copyleft (C) 2007 Eric KEDJI <eric_kedji@yahoo.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdarg.h>

#ifndef PRETTY_IO_H
#define PRETTY_IO_H

extern void
generic_io(const char *begin_with, const char *end_with, const char *format_string, va_list argp);

extern void
titre(const char *format);

extern void
info(const char *format, ...);

extern void
invite(const char *format, ...);

extern void
erreur(const char *format, ...);

extern void
get_line(char *prompt, char *line);

extern void vider_stdin(void);

extern void pause(void);

#endif /* PRETTY_IO_H */

