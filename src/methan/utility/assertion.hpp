#pragma once

#include <methan/core/except.hpp>
#include <methan/utility/exception.hpp>

#define METHAN_FORCE_ASSERT(condition, type, msg)                              METHAN_ECAPSULATE_LINE_DETAILS(if(!(condition)) { METHAN_THROW_EXCEPTION(msg, type); })
#define METHAN_FORCE_ASSERT_ARGUMENT(condition)                                METHAN_FORCE_ASSERT(condition, Methan::ExceptionType::IllegalArgument, "The condition \"" METHAN_STRINGIZE(condition) "\" failed")
#define METHAN_FORCE_ASSERT_NON_NULL(pointer)                                  METHAN_FORCE_ASSERT(pointer != nullptr, Methan::ExceptionType::NullPointer, "The pointer " METHAN_STRINGIZE(pointer) " should not be null")
#define METHAN_FORCE_ASSERT_NULL(pointer)                                      METHAN_FORCE_ASSERT(pointer == nullptr, Methan::ExceptionType::AlreadyInitialized, "Cannot reinitialize the pointer " METHAN_STRINGIZE(pointer) " that should be null")
#define METHAN_FORCE_ASSERT_INDEX(index, upperBound)                           METHAN_FORCE_ASSERT(index < upperBound, Methan::ExceptionType::IndexOutOfBounds, "The given index is out of bounds (" + std::to_string(index) + " should be less than " + std::to_string(upperBound) + ").")

#if defined(METHAN_DEBUG) || defined(METHAN_FORCE_ASSERTION)
#define METHAN_EXPAND_ASSERTION
#define METHAN_ASSERT(condition, type, msg)                                    METHAN_FORCE_ASSERT(condition, type, msg)
#define METHAN_ASSERT_ARGUMENT(condition)                                      METHAN_FORCE_ASSERT_ARGUMENT(condition)
#define METHAN_ASSERT_NON_NULL(pointer)                                        METHAN_FORCE_ASSERT_NON_NULL(pointer)
#define METHAN_ASSERT_NULL(pointer)                                            METHAN_FORCE_ASSERT_NULL(pointer)
#define METHAN_ASSERT_INDEX(index, upperBound)                                 METHAN_FORCE_ASSERT_INDEX(index, upperBound)
#else
#define METHAN_ASSERT(condition, type, msg)
#define METHAN_ASSERT_ARGUMENT(condition)
#define METHAN_ASSERT_NON_NULL(pointer)
#define METHAN_ASSERT_NULL(pointer)
#define METHAN_ASSERT_INDEX(index, upperBound)
#endif
