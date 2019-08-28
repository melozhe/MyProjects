// $Id: commands.cpp,v 1.17 2018-01-25 14:02:55-08 - - $

#include <iomanip>
#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"   , fn_rmr   }
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   if(cmd == "#") return nullptr;
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // words size exception check to add
   inode_ptr plainfile;
   wordvec data;
   dir_basename path;
   // cat all the files specified in command args
   for(size_t i=1; i<words.size(); ++i){
      // find the dir that the file sits
      path = state.find(words.at(i));
      plainfile = path.dir->get_contents()
            ->find(path.basename);
      // read data from the file
      data = plainfile->get_contents()->readfile();
      cout << data << endl;
   }
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() == 1 || words.at(1) == "/"){
      state.set_cwd(state.get_root());
      state.reset_pwd();
      return;
   }
   dir_basename path = state.find(words.at(1));
   inode_ptr target = path.dir->get_contents()
         ->find(path.basename);      
   // what if target is not a dir
   if(target->filetype() == file_type::PLAIN_TYPE){
      throw command_error(path.basename + ": is a plain file");
   } 
   state.set_cwd(target);
   state.chpwd(words.at(1));
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   int exit_num;
   if(words.size()==1) exit_num = 0; 
   else exit_num = stoi(words.at(1)); 
   exit_status::set(exit_num);
   throw ysh_exit();
}

wordvec updatepath(inode_state& state, const string& pathname){
   wordvec dirnames = split(pathname, "/");
   wordvec path;

   if(pathname.front() == '/') path.push_back("/");
   else path = state.pathname();

   for(size_t i=0; i<dirnames.size(); ++i){
      if(dirnames.at(i)==".") continue;
      else if(dirnames.at(i)== ".."){ 
         if(path.size()>1) path.pop_back();
      }else 
         path.push_back(dirnames.at(i));
   }
   return path;
}

void printpath(const wordvec& path){
   for(size_t i=0; i<path.size(); ++i){
      if(i>=2) cout << "/";
      cout << path.at(i);
   }
   cout << endl;
}

void printdir(inode_ptr target){
   // print out the contents in the dir
   auto itor = target->get_contents()->begin();
   auto end = target->get_contents()->end();
   for(;itor != end; ++itor){
      cout << setw(6) << itor->second->get_inode_nr() << "  ";
      cout << setw(6) << itor->second->get_contents()->size() << "  ";
      cout << itor->first << endl;
   }
}

void print_all(const wordvec& pathname, inode_ptr target){
   printpath(pathname);
   printdir(target);
   auto itor = target->get_contents()->begin();
   auto end = target->get_contents()->end();
   for(;itor != end; ++itor){
      if(itor->first != "." && itor->first != ".."){
         wordvec newpath = pathname;
         newpath.push_back(itor->first);
         print_all(newpath, itor->second);
      }
   }
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size()==1 || words.at(1) == "/") {
      printpath(state.pathname());
      // TODO(zhuang90): ls / is a different case
      printdir(state.get_cwd()); 
      return;
   }
   // find inode of the target dir
   dir_basename path = state.find(words.at(1));
   inode_ptr target = path.dir->get_contents()->find(path.basename);
   // print pathname and contents of the directory    
   wordvec newpath = updatepath(state, words.at(1));
   printpath(newpath);
   printdir(target);   
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   //wordvec newpath = updatepath(state, words.at(1));
   if(words.size()==1){
      print_all(state.pathname(), state.get_cwd());
   }else if(words.at(1) == "/"){
      print_all(wordvec{"/"},state.get_root());
   }
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // find the target dir to create new file
   dir_basename path = state.find(words.at(1));
   // call mkfile() to create new file   
   inode_ptr newfile = path.dir->get_contents()->mkfile(path.basename);
   newfile->get_contents()->writefile(words);
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // find the target dir to create new dir
   dir_basename path = state.find(words.at(1));
   // call mkdir() to create new dir
   inode_ptr newdir = path.dir->get_contents()
         ->mkdir(path.basename);
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   state.propmt(words);
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   printpath(state.pathname());
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // decode pathname and identify the directory
   // that the file live in
   dir_basename path = state.find(words.at(1));
   // call remove on that dir
   path.dir->get_contents()->remove(path.basename);
   
}

void delete_all(inode_ptr location, const string& basename){
   inode_ptr target = location->get_contents()->find(basename);
   auto itor = target->get_contents()->begin();
   auto end = target->get_contents()->end();
   for(;itor != end; ++itor){
      if(itor->first == "." || itor->first == ".."
         || itor->second->filetype()==file_type::PLAIN_TYPE){
         target->get_contents()->erase(itor->first);
      }else{
         delete_all(target, itor->first);
      }
   }
   location->get_contents()->erase(basename);
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.at(1) == "/"){
      delete_all(state.get_root(),".");
   }
   dir_basename path = state.find(words.at(1));
   delete_all(path.dir, path.basename);
}

