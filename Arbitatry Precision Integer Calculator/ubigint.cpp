// $Id: ubigint.cpp,v 1.16 2019-04-02 16:28:42-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "ubigint.h"
#include "debug.h"

ubigint::ubigint (unsigned long that){
   DEBUGF ('~', this << " -> " << that)
   udigit_t r;
   if(that==0) ubig_value.push_back(0);
   else{
      while(that !=0){
         r = that % 10;
         ubig_value.push_back(r);
         that /= 10;
      }
   }
}

ubigint::ubigint (const string& that) {
   DEBUGF ('~', "that = \"" << that << "\"");
   for(int i=that.size()-1; i>=0; --i){
      if(not isdigit(that[i])){
         throw invalid_argument("ubigint::ubigint(" + that + ")");
      }
      ubig_value.push_back(that[i]-'0');
   }
   while(ubig_value.size()>0 && ubig_value.back()==0) ubig_value.pop_back();
}

ubigint ubigint::operator+ (const ubigint& that) const {
   int lsize = ubig_value.size();
   int rsize = that.ubig_value.size();
   int size = lsize>rsize?lsize:rsize;
   int carry = 0;
   udigit_t ldigit,rdigit,digit;
   ubigint result;
   for(int i=0; i<size; ++i){
      ldigit = i>=lsize?0:ubig_value[i];
      rdigit = i>=rsize?0:that.ubig_value[i];
      
      digit = (ldigit + rdigit + carry)%10;
      carry = (ldigit + rdigit + carry)/10;
      
      result.ubig_value.push_back(digit);
   }
   if(carry)
      result.ubig_value.push_back(carry);
   
   return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   int size = ubig_value.size();
   int rsize = that.ubig_value.size();
   int borrow = 0;
   int ldigit,rdigit,digit;
   ubigint result;
   for(int i=0; i<size; ++i){
      ldigit = ubig_value[i]+borrow;
      rdigit = i>=rsize?0:that.ubig_value[i];

      if(ldigit<rdigit){
         ldigit += 10;
	 borrow = -1;
      }else{
         borrow = 0;
      }

      digit = ldigit - rdigit;
      result.ubig_value.push_back(digit);
   }
   while(result.ubig_value.size()>1 && result.ubig_value.back()==0) 
      result.ubig_value.pop_back(); 	
   return result;
}

ubigint ubigint::operator* (const ubigint& that) const {
   int lsize = ubig_value.size();
   int rsize = that.ubig_value.size();
   udigit_t carry = 0, digit, ldigit, rdigit;
   ubigint result, tmp;
   for(int j=0; j<rsize; ++j){
      tmp.ubig_value.clear();	   
      for(int k=0;k<j;++k)
         tmp.ubig_value.push_back(0);
      rdigit = that.ubig_value[j];
      for(int i=0; i<lsize; ++i){
         ldigit = ubig_value[i];
	 digit = (rdigit * ldigit + carry)%10;
	 carry = (rdigit * ldigit + carry)/10;
	 tmp.ubig_value.push_back(digit);
      }
      if(carry){
         tmp.ubig_value.push_back(carry);
         carry = 0;
      }
      result = result + tmp;
   }
   return result;
}

void ubigint::multiply_by_2() {
   (*this) = (*this) * 2;
}

void ubigint::divide_by_2() {
   int size = ubig_value.size();
   udigit_t digit, carry=0;
   for(int i=size-1;i>=0;--i){
      digit = (ubig_value[i] + carry)/2;
      carry = (ubig_value[i] + carry)%2*10;
      ubig_value[i] = digit;
   }
   while(ubig_value.size()>0 && ubig_value.back()==0) 
      ubig_value.pop_back(); 	
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   // NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   int lsize = ubig_value.size();
   int rsize = that.ubig_value.size();
   bool eq = (lsize == rsize);
   udigit_t ldigit, rdigit;
   int i=0;
   while(eq && i<lsize){
      ldigit = ubig_value[i];
      rdigit = that.ubig_value[i];
      eq = (ldigit == rdigit);
      ++i;
   }
   
   return eq;
}

bool ubigint::operator< (const ubigint& that) const {
   int lsize = ubig_value.size();
   int rsize = that.ubig_value.size();
   if(lsize < rsize) return true;
   else if(lsize > rsize) return false;
   else{
      for(int i=lsize-1;i>=0;--i){
         if(ubig_value[i]>that.ubig_value[i]) return false;
	 else if(ubig_value[i]<that.ubig_value[i]) return true;
      }	
   }
   return false;
}

ostream& operator<< (ostream& out, const ubigint& that) { 
   for(int i=that.ubig_value.size()-1;i>=0;--i){
      out << static_cast<char>(that.ubig_value[i]+'0');
   }
   return out;
}
