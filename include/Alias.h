
template<class T>
using OnClickListener = std::function<void(T)>;

template<class P>
using PointerArray = std::vector<std::shared_ptr<P>>;