// This program demonstrates access control in classes.

struct Publ {
	virtual ~Publ() {}
};

struct Prot {
	virtual ~Prot() {}
};

struct Priv {
	virtual ~Priv() {}
};

struct A: public Publ, protected Prot, private Priv {
public:
	int publ;
protected:
	int prot;
private:
	int priv;
// public: Accessible anywhere (default if A is defined with "struct")
// protected: Accessible by this class, and any derived class (with the restriction below)
// private: Accessible by this class only (default if A is defined with "class")

// Restriction for protected access: if a non-static protected member is accessed by a derived class,
// the type of the object where the member is accessed must be the derived class, or its descendents.
// See C++ standard section [class.protected].
public:
	void func() {
		Publ &base1 = *this;
		publ = 0;
		Prot &base2 = *this;
		prot = 0;
		Priv &base3 = *this;
		priv = 0;
	}
	friend void fridA(A &a) {
		Publ &base1 = a;
		a.publ = 0;
		Prot &base2 = a;
		a.prot = 0;
		Priv &base3 = a;
		a.priv = 0;
	}
};

struct dervA: A {
	void func() {
		A &a = *this;
		Publ &base1 = a;
		publ = 0;
		Prot &base2 = a;
		prot = 0;
		//Priv &base3 = a; // Error
		//priv = 0; // Error
	}
};

void funcA(A &a) {
	Publ &base1 = a;
	a.publ = 0;
	//Prot &base2 = a; // Error
	//a.prot = 0; // Error
	//Priv &base3 = a; // Error
	//a.priv = 0; // Error
}

// ----------------------------------------------

struct B: public A {
// Things that are public or protected in A are inherited as is
// (default if B is defined with "struct")
	void func() {
		Publ &base1 = *this;
		publ = 0;
		Prot &base2 = *this;
		prot = 0;
	}
	friend void fridB(B &b) {
		Publ &base1 = b;
		b.publ = 0;
		Prot &base2 = b;
		b.prot = 0;
	}
};

struct dervB: B {
	void func() {
		B &b = *this;
		Publ &base1 = b;
		publ = 0;
		Prot &base2 = b;
		prot = 0;
	}
};

void funcB(B &b) {
	Publ &base1 = b;
	b.publ = 0;
	//Prot &base2 = b; // Error
	//b.prot = 0; // Error
}

// ----------------------------------------------

struct C: protected A {
// Things that are public or protected in A are inherited as protected
	void func() {
		Publ &base1 = *this;
		publ = 0;
		Prot &base2 = *this;
		prot = 0;
	}
	friend void fridC(C &c) {
		Publ &base1 = c;
		c.publ = 0;
		Prot &base2 = c;
		c.prot = 0;
	}
};

struct dervC: C {
	void func() {
		C &c = *this;
		Publ &base1 = c;
		publ = 0;
		Prot &base2 = c;
		prot = 0;
	}
};

void funcC(C &c) {
	//Publ &base1 = c; // Error
	//c.publ = 0; // Error
	//Prot &base2 = c; // Error
	//c.prot = 0; // Error
}

// ----------------------------------------------

struct D: private A {
// Things that are public or protected in A are inherited as private
// (default if D is defined with "class")
	void func() {
		Publ &base1 = *this;
		publ = 0;
		Prot &base2 = *this;
		prot = 0;
	}
	friend void fridD(D &d) {
		Publ &base1 = d;
		d.publ = 0;
		Prot &base2 = d;
		d.prot = 0;
	}
};

struct dervD: D {
	void func() {
		D &d = *this;
		//Publ &base1 = d; // Error
		//publ = 0; // Error
		//Prot &base2 = d; // Error
		//prot = 0; // Error
	}
};

void funcD(D &d) {
	//Publ &base1 = d; // Error
	//d.publ = 0; // Error
	//Prot &base2 = d; // Error
	//d.prot = 0; // Error
}

// ----------------------------------------------

int main() {
	return 0;
}
