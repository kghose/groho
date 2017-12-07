#pragma once
#include <array>
#include <vector>
#include <memory>


namespace sim
{
/*
template<class T, size_t N>
struct Node
{
  T data[ N ];
  std::atomic<size_t> size = 0;
  // How we keep track of how many points filled
};
*/
template<class T, size_t N, size_t M>
struct NodeArray
{
  NodeArray()
  {
    nodes[ last_node_with_data ] = std::unique_ptr<T[]>( new T[ N ] );
  }

  void 
  append( const T val )
  // place at next available location. Add a new node if needed
  {
    if( last_node_next_index == N ) {
      last_node_with_data++;
      nodes[ last_node_with_data ] = std::unique_ptr<T[]>( new T[ N ] );
      last_node_next_index = 0;
    } 
    nodes[ last_node_with_data ][ last_node_next_index ] = val; 
    last_node_next_index.store( 
      last_node_next_index + 1,
      std::memory_order_release);
  }

  size_t 
  size() const
  // Total accessible elements in the nodearray
  {
    return last_node_with_data * N + last_node_next_index.load( std::memory_order_acquire );
  }

  const T& 
  operator[]( size_t idx ) const
  // Compute the appropriate strides etc and give us the correct value
  // No bounds checking for speed
  {
    size_t m = idx / N;
    size_t n = idx % N;
    return nodes[ m ][ n ];
  }

private:
  std::unique_ptr<T[]> nodes[ M ];
  std::atomic<size_t> last_node_with_data = 0;
  std::atomic<size_t> last_node_next_index = 0;
  // How many nodes in the array are at least partially filled
};

} // namespace sim