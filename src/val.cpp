#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <iomanip>
#include <stdexcept>
#include <cmath>
#include <sstream>
#include <cmath>
#include "val.h"

using namespace std;

Value Value::operator*(const Value& op) const{
    Value final;
    if((this -> IsInt() || this -> IsReal()) && (op.IsInt() || op.IsReal())){
    
        if (this -> IsInt() && op.IsInt()){
            final = Value(this -> GetInt() * op.GetInt());
        }
        else if(this -> IsReal() && op.IsReal()){
            final = Value(this -> GetReal() * op.GetReal());
        }
        else if(this -> IsReal() && op.IsInt()){
            final = Value(this -> GetReal() * op.GetInt());
        }
        else if(this -> IsInt() && op.IsReal()){
            final = Value(this -> GetInt() * op.GetReal());
        }
    }
    else{
        final = Value();
    }
    return final;
     
}

Value Value::operator/(const Value& op) const{
    Value final;
    if((this -> IsInt() || this -> IsReal()) && (op.IsInt() || op.IsReal())){
    
        if (this -> IsInt() && op.IsInt()){
            final = Value(this -> GetInt() / op.GetInt());
        }
        else if(this -> IsReal() && op.IsReal()){
            final = Value(this -> GetReal() / op.GetReal());
        }
        else if(this -> IsReal() && op.IsInt()){
            final = Value(this -> GetReal() / op.GetInt());
        }
        else if(this -> IsInt() && op.IsReal()){
            final = Value(this -> GetInt() / op.GetReal());
        }
    }
    else{
        final = Value();
    }
    return final;
     
}
Value Value::operator+(const Value& op) const{
    Value final;
    if((this -> IsInt() || this -> IsReal()) && (op.IsInt() || op.IsReal())){
    
        if (this -> IsInt() && op.IsInt()){
            final = Value(this -> GetInt() + op.GetInt());
        }
        else if(this -> IsReal() && op.IsReal()){
            final = Value(this -> GetReal() + op.GetReal());
        }
        else if(this -> IsReal() && op.IsInt()){
            final = Value(this -> GetReal() + op.GetInt());
        }
        else if(this -> IsInt() && op.IsReal()){
            final = Value(this -> GetInt() + op.GetReal());
        }
    }
    else{
        final = Value();
    }
    return final;
     
}
Value Value::operator-(const Value& op) const{
    Value final;
    if((this -> IsInt() || this -> IsReal()) && (op.IsInt() || op.IsReal())){
    
        if (this -> IsInt() && op.IsInt()){
            final = Value(this -> GetInt() - op.GetInt());
        }
        else if(this -> IsReal() && op.IsReal()){
            final = Value(this -> GetReal() - op.GetReal());
        }
        else if(this -> IsReal() && op.IsInt()){
            final = Value(this -> GetReal() - op.GetInt());
        }
        else if(this -> IsInt() && op.IsReal()){
            final = Value(this -> GetInt() - op.GetReal());
        }
    }
    else{
        final = Value();
    }
    return final;
     
}

Value Value::operator<(const Value& op) const{
    Value final;
    if((this -> IsInt() || this -> IsReal()) && (op.IsInt() || op.IsReal())){
        if (this -> IsInt() && op.IsInt()){
            final = Value(this -> GetInt() < op.GetInt());
        }
        else if(this -> IsReal() && op.IsReal()){
            final = Value(this -> GetReal() < op.GetReal());
        }
        else if(this -> IsReal() && op.IsInt()){
            final = Value(this -> GetReal() < (op.GetInt() * 1.0));
        }
        else if(this -> IsInt() && op.IsReal()){
            final = Value((this -> GetInt()* 1.0) < op.GetReal());
        }
    }
    else{
        final = Value();
    }
    return final;
}
Value Value::operator>(const Value& op) const{
    Value final;
    if((this -> IsInt() || this -> IsReal()) && (op.IsInt() || op.IsReal())){
        if (this -> IsInt() && op.IsInt()){
            final = Value(this -> GetInt() > op.GetInt());
        }
        else if(this -> IsReal() && op.IsReal()){
            final = Value(this -> GetReal() > op.GetReal());
        }
        else if(this -> IsReal() && op.IsInt()){
            final = Value(this -> GetReal() > (op.GetInt() * 1.0));
        }
        else if(this -> IsInt() && op.IsReal()){
            final = Value((this -> GetInt()* 1.0) > op.GetReal());
        }
    }
    else{
        final = Value();
    }
    return final;
}
Value Value::operator==(const Value& op) const{
    Value final;
    if((this -> IsInt() || this -> IsReal()) && (op.IsInt() || op.IsReal())){
        if (this -> IsInt() && op.IsInt()){
            final = Value(this -> GetInt() == op.GetInt());
        }
        else if(this -> IsReal() && op.IsReal()){
            final = Value(this -> GetReal() == op.GetReal());
        }
        else if(this -> IsReal() && op.IsInt()){
            final = Value(this -> GetReal() == (op.GetInt() * 1.0));
        }
        else if(this -> IsInt() && op.IsReal()){
            final = Value((this -> GetInt()* 1.0) == op.GetReal());
        }
    }
    else{
        final = Value();
    }
    return final;
}

Value Value::Catenate(const Value & op) const{
    Value final;
    if(this -> IsString() && op.IsString()){
        final = Value(this -> GetString() + op.GetString());
    }
    else{
        final = Value();
    }
    return final;
}

Value Value::Power(const Value & op) const{
    Value final;
    if (this -> IsInt() && op.IsInt()){
        final = Value(pow((this -> GetInt() * 1.0), (op.GetInt() * 1.0)));
    }   
    else if(this -> IsReal() && op.IsReal()){
        final = Value(pow(this -> GetReal(), op.GetReal()));
    }
    else if(this -> IsReal() && op.IsInt()){
        final = Value(pow(this -> GetReal(), (op.GetInt() * 1.0)));
    }
    else if(this -> IsInt() && op.IsReal()){
        final = Value(pow((this -> GetInt() * 1.0), op.GetReal()));
    }
    else{
        final = Value();
    }
    return final;
}