// $Id: interp.cpp,v 1.3 2019-03-19 16:18:22-07 - - $

#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

unordered_map<string,interpreter::interpreterfn>
interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"border" , &interpreter::do_border },
   {"moveby" , &interpreter::do_moveby },

};

unordered_map<string,interpreter::factoryfn>
interpreter::factory_map {
   {"text"     , &interpreter::make_text     },
   {"ellipse"  , &interpreter::make_ellipse  },
   {"circle"   , &interpreter::make_circle   },
   {"polygon"  , &interpreter::make_polygon  },
   {"rectangle", &interpreter::make_rectangle},
   {"square"   , &interpreter::make_square   },
   {"diamond"  , &interpreter::make_diamond  },
   {"triangle" , &interpreter::make_triangle },
   {"equilateral" , &interpreter::make_equilateral },

};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   func (++begin, params.cend());
}

void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   objmap.emplace (name, make_shape (++begin, end));
}

void interpreter::do_border (param begin, param end){
   DEBUGF ('f', range (begin, end));
   window::setbordercolor(rgbcolor(*begin++));
   window::setborderwidth(from_string<GLfloat>(*begin));
}

void interpreter::do_moveby (param begin, param end){
   DEBUGF ('f', range (begin, end));
   window::setmoveby(from_string<int>(*begin));
}

void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 4) throw runtime_error ("syntax error");
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      throw runtime_error (name + ": no such shape");
   }
   rgbcolor color {begin[0]};
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   window::push_back (object (itor->second, where, color));
}

shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return func (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   // TODO?
   string font = *begin++;
   auto result = fontcode.find(font);
   if(result == fontcode.end()){
      throw runtime_error(font + ": no such font");
   }
   string contents = *begin++;
   for(auto itor = begin; itor != end; ++itor){
      (contents += " ") += *itor;
   }
   // pass them to the following func
   return make_shared<text> (result->second, contents);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat width = from_string<GLfloat> (begin[0]);
   GLfloat height = from_string<GLfloat> (begin[1]);
   return make_shared<ellipse> (width, height);
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat radius = from_string<GLfloat> (begin[0]);
   return make_shared<circle> (radius);
}

void interpreter::normalize_vertices(param begin, param end, 
                                       vertex_list& vertices){
   int n = end - begin;
   int sum_x = 0, sum_y = 0;
   for (int i=0; i < n; i++){
      GLfloat x_pos = from_string<GLfloat> (begin[i]);
      GLfloat y_pos = from_string<GLfloat> (begin[++i]);
      vertex ver {x_pos, y_pos};
      vertices.push_back(ver);
      sum_x += x_pos;
      sum_y += y_pos;
   }
   int avg_x = sum_x / vertices.size() ;
   int avg_y = sum_y / vertices.size() ;
   for (size_t i=0; i < vertices.size(); i++){
      vertices[i].xpos -= avg_x;
      vertices[i].ypos -= avg_y;
   }
}
shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   // calculate the average of vertices
   vertex_list vertices;
   normalize_vertices(begin, end, vertices);
   return make_shared<polygon> (vertices);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat width = from_string<GLfloat> (begin[0]);
   GLfloat height = from_string<GLfloat> (begin[1]);
   return make_shared<rectangle> (width, height);
}  

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<square> (from_string<GLfloat> (begin[0]));
}

shape_ptr interpreter::make_diamond (param begin, param end){
   DEBUGF ('f', range (begin, end));
   GLfloat width = from_string<GLfloat> (begin[0]);
   GLfloat height = from_string<GLfloat> (begin[1]);
   return make_shared<diamond> (width, height);
}

shape_ptr interpreter::make_triangle(param begin, param end){
   vertex_list vertices;
   normalize_vertices(begin, end, vertices);
   return make_shared<triangle> (vertices);
}

shape_ptr interpreter::make_equilateral(param begin, param end){
   DEBUGF ('f', range (begin, end));
   vertex_list vertices;
   return make_shared<equilateral> (from_string<GLfloat> (begin[0]));
}

