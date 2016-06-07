#include <iostream>
#include <exception>
#include <stdexcept>
#include <sstream>
#include <string>

namespace Z {

class NoCallbackError: public std::logic_error {
    public:
        NoCallbackError(std::string cb) : logic_error("Necessary callback missing"), callbackName(cb) {}

        virtual const char* what() const throw() {
            cnvt.str("");
            cnvt << logic_error::what() << ": " << callbackName;
            return cnvt.str().c_str();
        }

    private:
        std::string callbackName;
        static std::ostringstream cnvt;

};

std::ostringstream NoCallbackError::cnvt;

};
