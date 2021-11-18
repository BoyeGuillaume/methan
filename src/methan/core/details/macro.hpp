#pragma once

#define __METHAN_EXPAND_DETAILS(x)                                   x
#define __METHAN_CONCATENATE_DETAILS(a, b)                           a ## b
#define __METHAN_STRINGIZE_DETAILS(x)                                #x
#define __METHAN_ECAPSULATE_LINE_DETAILS(line)                       do { line } while(0)
