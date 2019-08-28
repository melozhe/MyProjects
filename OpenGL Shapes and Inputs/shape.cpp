// $Id: shape.cpp,v 1.2 2019-02-28 15:24:20-08 - - $
#define _USE_MATH_DEFINES
#include <typeinfo>
#include <unordered_map>
#include <cmath>
using namespace std;

#include "shape.h"
#include "util.h"

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   // What's here?
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font_, const string& textdata_):
      glut_bitmap_font(glut_bitmap_font_), textdata(textdata_) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}


polygon::polygon (const vertex_list& vertices_): vertices(vertices_) {
   DEBUGF ('c', this);
}

// vertex_list make_vertices(GLfloat width, GLfloat height)
// {
// }

rectangle::rectangle (GLfloat width, GLfloat height):
         polygon({{width/2, height/2}, {width/2, -height/2}, 
                  {-width/2, -height/2}, {-width/2, height/2}}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

diamond::diamond (GLfloat width, GLfloat height): 
         polygon({{width/2,0}, {0, -height/2}, 
                  {-width/2, 0}, {0, height/2}}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

triangle::triangle (const vertex_list& vertices_): polygon(vertices_){
   DEBUGF ('c', this);
}

equilateral::equilateral (GLfloat edge): triangle({{edge/2,-sqrtf(3)/6*edge},
                        {-edge/2, -sqrtf(3)/6*edge}, {0,sqrtf(3)/3*edge}}){
   DEBUGF ('c', this);
}

void draw_number(const vertex& center, size_t num){
   rgbcolor numcolor {};
   string num_str = to_string<int>(num);
   auto num_ptr = reinterpret_cast<const GLubyte *>(num_str.c_str());
   int width = glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, num_ptr);
   int height = glutBitmapHeight(GLUT_BITMAP_TIMES_ROMAN_24);
   glColor3ubv(numcolor.ubvec);
   glRasterPos2f(center.xpos - width/2, center.ypos- height/4);
   glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, num_ptr);
}

void text::draw (const vertex& center, const rgbcolor& color, 
                     size_t num) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   // draw text at specified position in specified  color
   auto text_ptr = reinterpret_cast<const GLubyte *>(textdata.c_str());
   glColor3ubv(color.ubvec);
   float xpos = center.xpos;
   float ypos = center.ypos;
   glRasterPos2f(xpos, ypos);
   glutBitmapString(glut_bitmap_font, text_ptr);
   // draw num on center
   draw_number(center, num);
}

void ellipse::draw (const vertex& center, const rgbcolor& color, 
                     size_t num) const {
   // TODO 
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin(GL_POLYGON);
   glColor3ubv(color.ubvec);
   const float delta = 2 * M_PI / 32;
   for (float theta = 0; theta <= 2 * M_PI ; theta += delta){
      float xpos = center.xpos + dimension.xpos / 2 * cos(theta);
      float ypos = center.ypos + dimension.ypos / 2 * sin(theta);
      glVertex2f(xpos, ypos);
   }
   glEnd();
   // draw num on center
   draw_number(center, num);
   
}

void ellipse::draw_border (const vertex& center, const rgbcolor& color,
                              GLbyte borderwidth) const {
   glLineWidth(borderwidth);
   glBegin(GL_LINE_LOOP);
   glColor3ubv(color.ubvec);
   const float delta = 2 * M_PI / 32;
   for (float theta = 0; theta <= 2 * M_PI; theta += delta){
      float xpos = center.xpos + dimension.xpos / 2 * cos(theta);
      float ypos = center.ypos + dimension.ypos / 2 * sin(theta);
      glVertex2f(xpos, ypos);
   }
   glEnd();
}

void polygon::draw (const vertex& center, const rgbcolor& color, 
                     size_t num) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin(GL_POLYGON);
   glColor3ubv (color.ubvec);
   for (size_t i=0; i < vertices.size(); ++i){
      glVertex2f (center.xpos + vertices[i].xpos, 
                  center.ypos + vertices[i].ypos);
   }
   glEnd();
   // draw num on center
   draw_number(center, num);
}

void polygon::draw_border (const vertex& center, const rgbcolor& color,
                              GLbyte borderwidth) const {
   glLineWidth(borderwidth);
   glBegin(GL_LINE_LOOP);
   glColor3ubv(color.ubvec);
   for (size_t i = 0; i < vertices.size(); ++i){
      glVertex2f(center.xpos + vertices[i].xpos,
                 center.ypos + vertices[i].ypos);
   }
   glEnd();
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

