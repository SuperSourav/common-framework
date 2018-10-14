/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Containers_HH
#define Containers_HH

#include <memory>
#include <string>
#include <iostream>


namespace TTHbb{

  template <class T>
  class Container :public std::vector<std::shared_ptr<T> > {

  public:

    Container();
    virtual ~Container();
    std::string name;

    Container* clone() const; /// pointers are the same!
    Container* deepClone() const;
    void dump() const;

    void add(T*);
    void swap(int i, int j); 
    void replace(int i, T*); 

  private:

    static int NbrContainer;

  };

  template <class T>
  int Container<T>::NbrContainer = 0;

  template <class T>
  Container<T>::Container(){

    name="";
    ++NbrContainer;

  }

  template <class T>
  Container<T>::~Container(){
    --NbrContainer;
  }


  template <class T>
  void Container<T>::add(T* p) {
    this->push_back(std::shared_ptr<T>(p));
  }


  template <class T>
  void Container<T>::replace(int i, T* t){
    (*this)[i] = std::shared_ptr<T>(t);
  }

  template <class T>
  void Container<T>::swap(int i, int j){
    std::shared_ptr<T> hold = (*this)[i];
    (*this)[i]=(*this)[j];
    (*this)[j]=hold;
  } 


  template <class T>
  void Container<T>::dump() const{
    std::cout << " - Container: " << name << "[size " << this->size() << "] : ";
    std::cout << std::endl;
    for(auto a : *this){
      std::cout << " --- { ";
      a->dump();
      std::cout << " } " << std::endl;
    }
    std::cout << std::endl;

  }



  template <class T>
  Container<T>* Container<T>::clone() const{
    return new Container<T>(*this);
  }

  template <class T>
  Container<T>* Container<T>::deepClone() const{
  
    Container<T> *n = new Container<T>();  
    n->resize(this->size());

    for(unsigned int i=0; i<this->size(); ++i){
      (*n)[i] = std::shared_ptr<T>((*this)[i]->deepClone());
    }

    return n;

  }

}

#endif
