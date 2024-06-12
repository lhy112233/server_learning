#ifndef HY_EXCEPTIONWRAPPER_H_
#define HY_EXCEPTIONWRAPPER_H_

#include <string_view>
class ExceptionWrapper final{
    private:
    struct WithExceptionFromFn_;
    struct WithExceptionFromEx_;

    [[noreturn]] static void onNoExceptionError(std::string_view name);

};  //class ExceptionWrapper







#endif  //HY_EXCEPTIONWRAPPER_H_