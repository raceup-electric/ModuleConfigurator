#pragma once

// NOTE: This could achieved in other ways:
//       -the most idiomatic one is variadic friend templates, that are in c++26 
//       -with some macros, but you would need recursion in preprocessor,
//       e.g. with boost 
//       -if there will ever be a reverse_protected but I don't guess so...

namespace ru::capability {

// 1 friend
template <typename A>
class Token1 {
 private:
  Token1() = default;
  friend A;
};

// 2 friends
template <typename A, typename B>
class Token2 {
 private:
  Token2() = default;
  friend A;
  friend B;
};

// 3 friends
template <typename A, typename B, typename C>
class Token3 {
 private:
  Token3() = default;
  friend A;
  friend B;
  friend C;
};

// 4 friends
template <typename A, typename B, typename C, typename D>
class Token4 {
 private:
  Token4() = default;
  friend A;
  friend B;
  friend C;
  friend D;
};

// 5 friends
template <typename A, typename B, typename C, typename D, typename E>
class Token5 {
 private:
  Token5() = default;
  friend A;
  friend B;
  friend C;
  friend D;
  friend E;
};

// 6 friends
template <typename A, typename B, typename C, typename D, typename E,
          typename F>
class Token6 {
 private:
  Token6() = default;
  friend A;
  friend B;
  friend C;
  friend D;
  friend E;
  friend F;
};

// 7 friends
template <typename A, typename B, typename C, typename D, typename E,
          typename F, typename G>
class Token7 {
 private:
  Token7() = default;
  friend A;
  friend B;
  friend C;
  friend D;
  friend E;
  friend F;
  friend G;
};

// 8 friends
template <typename A, typename B, typename C, typename D, typename E,
          typename F, typename G, typename H>
class Token8 {
 private:
  Token8() = default;
  friend A;
  friend B;
  friend C;
  friend D;
  friend E;
  friend F;
  friend G;
  friend H;
};

// 9 friends
template <typename A, typename B, typename C, typename D, typename E,
          typename F, typename G, typename H, typename I>
class Token9 {
 private:
  Token9() = default;
  friend A;
  friend B;
  friend C;
  friend D;
  friend E;
  friend F;
  friend G;
  friend H;
  friend I;
};

// 10 friends
template <typename A, typename B, typename C, typename D, typename E,
          typename F, typename G, typename H, typename I, typename J>
class Token10 {
 private:
  Token10() = default;
  friend A;
  friend B;
  friend C;
  friend D;
  friend E;
  friend F;
  friend G;
  friend H;
  friend I;
  friend J;
};

}  // namespace ru::capability
