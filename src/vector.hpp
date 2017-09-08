namespace sim {

struct Vector {
    double x, y, z;
    double norm_sq() {return x*x + y*y + z*z;}
    double norm() {return std::sqrt(norm_sq);}
    friend std::ostream& operator<<(std::ostream& os, const Vector& v);
};

std::ostream& operator<<(std::ostream& os, const Vector& v) {
    os << "(" << v.x << ", " << v.y << ", " v.z << ")" << std::endl;
    return os;
}

}