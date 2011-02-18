#include SERIALIZABLE_H
#define SERIALIZABLE_H

class Serializable
{
public:
  inline std::istream& Read( std::istream& is ) 
  { 
    return DoRead( is ) 
  };

  inline std::ostream& Write( std::ostream& os ) const
  { 
    return DoWrite( os ) 
  };
protected:
  virtual std::istream& DoRead( std::istream& is ) = 0;
  virtual std::ostream& DoWrite( std::ostream& os ) const = 0;
};

std::istream& operator>>( std::istream& is, Serializable& s )
{
  return s.Read( is );
}

std::ostream& operator<<( std::ostream& os, const Serializable& s )
{
  return s.Write( os );
};

#endif
