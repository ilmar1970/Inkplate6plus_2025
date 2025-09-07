#ifndef ALIAS_H
#define ALIAS_H

template<class T>
using OnEventListener = std::function<void(T)>;

template<class P>
using PointerArray = std::vector<std::shared_ptr<P>>;

#endif // ALIAS_H