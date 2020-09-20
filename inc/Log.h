/* Copyright © Artur Maziarek MMXX
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#pragma once

#include <stdio.h>

#if LOGLEVEL>=1
#define LOGE(...) printf("ERROR "); printf(__VA_ARGS__);
#else
#define LOGE(...)
#endif

#if LOGLEVEL>=2
#define LOGW(...) printf("WARNING "); printf(__VA_ARGS__);
#else
#define LOGW(...)
#endif

#if LOGLEVEL>=3
#define LOGI(...) printf("INFO "); printf(__VA_ARGS__);
#else
#define LOGI(...)
#endif

#if LOGLEVEL>=4
#define LOGD(...) printf("DEBUG "); printf(__VA_ARGS__);
#else
#define LOGD(...)
#endif
