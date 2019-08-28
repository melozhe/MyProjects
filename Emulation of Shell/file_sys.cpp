// $Id: file_sys.cpp,v 1.6 2018-06-27 14:44:57-07 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

#include "debug.h"
#include "file_sys.h"


int inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

inode_state::inode_state() {
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
   root = make_shared<inode>(file_type::DIRECTORY_TYPE);
   cwd = root;
   root->contents->initdir(root,root);
}

const string& inode_state::prompt() const { return prompt_; }
void inode_state::propmt(const wordvec& that){ 
   prompt_.clear();
   for(size_t i=1; i<that.size(); ++i) prompt_.append(that.at(i) + " ");
}

// decode a pathname like a/b/c/d. Special case when pathname only has
// a basename is handled. 
dir_basename inode_state::find(const string& pathname){
   wordvec dirnames = split(pathname, "/");
   inode_ptr dir;
   string basename = "";
   if(pathname.front() == '/') dir = root;
   else dir = cwd;
   ssize_t size = dirnames.size()-1;
   for(ssize_t i=0; i<size; ++i)
      dir = dir->contents->find(dirnames.at(i));
   basename = dirnames.back();
   return {dir,basename};
}

void inode_state::chpwd(const string& pathname){
   wordvec dirnames = split(pathname, "/");
   if(pathname.front() == '/') reset_pwd();
   for(size_t i=0; i<dirnames.size(); ++i){
      if(dirnames.at(i) == ".") continue;
      else if(dirnames.at(i) == ".."){
         if(pathname_.size()>1) pathname_.pop_back();
      }else{
         pathname_.push_back(dirnames.at(i));
      }
   }
}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           type_ = file_type::PLAIN_TYPE;
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           type_ = file_type::DIRECTORY_TYPE;
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

base_file_ptr inode::get_contents() const{
   return contents;
}

file_error::file_error (const string& what):
            runtime_error (what) {
}

size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   for(size_t i=0; i<data.size(); ++i){
      if(i != (data.size()-1)) ++size;
      size += data.at(i).length();
   }
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
   for(size_t i=2; i<words.size(); ++i){
      data.push_back(words.at(i));
   }
}

void plain_file::remove (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkdir (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkfile (const string&) {
   throw file_error ("is a plain file");
}

void plain_file::initdir(inode_ptr, inode_ptr) {
   throw file_error("is a plain file");
}

dir_itor plain_file::begin(){
   throw file_error("is a plain file");
}

dir_itor plain_file::end(){
   throw file_error("is a plain file");
}

inode_ptr plain_file::find(const string&){
   throw file_error("is a plain file");
}

void plain_file::erase(const string&){
   throw file_error("is a plain file");
}

size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   size = dirents.size();
   return size;
}

const wordvec& directory::readfile() const {
   throw file_error ("is a directory");
}

void directory::writefile (const wordvec&) {
   throw file_error ("is a directory");
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
   const auto result = dirents.find(filename);
   if(result == dirents.end()){
      throw file_error(filename + ": no such file");
   }
   if(result->second->type_ == file_type::DIRECTORY_TYPE
      && result->second->get_contents()->size()){
      throw file_error(filename + ": not empty directory");
   }
   dirents.erase(result);
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   const auto result = dirents.find(dirname);
   if(result != end()){
      throw file_error(dirname + ": file already exists");
   }
   inode_ptr newdir = make_shared<inode>(file_type::DIRECTORY_TYPE);
   newdir->contents->initdir(dirents.at("."), newdir);
   dirents.emplace(dirname, newdir);
   return newdir;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   const auto result = dirents.find(filename);
   if(result != dirents.end()){
      throw file_error(filename + ": file already exists");
   }
   inode_ptr newfile = make_shared<inode>(file_type::PLAIN_TYPE);
   dirents.emplace(filename, newfile);
   return newfile;
}

void directory::initdir(inode_ptr par, inode_ptr cur) {
   dirents.emplace(".", cur);
   dirents.emplace("..", par);
}

dir_itor directory::begin(){
   return dirents.begin();
}

dir_itor directory::end(){
   return dirents.end();
}

inode_ptr directory::find(const string& dirname){
   DEBUGF('i', dirname);
   const auto result = dirents.find(dirname);
   if(result == dirents.end()){
      throw file_error(dirname + ": no such file");
   }
   return result->second;  
}

void directory::erase(const string& filename){
   const auto result = dirents.find(filename);
   dirents.erase(result);
}
