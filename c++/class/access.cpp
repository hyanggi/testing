// This program demonstrates access control in classes.

/* ---------- Rules -----------

[class.access.base]

1.
Public inheritance: public members become public members; protected members become protected members.
Protected inheritance: public and protected members become protected members.
Private inheritance: public and protected members become private members.
In all cases: private members become inaccessible members; inaccessible members become inaccessible members.

4.
A base class B of class N is accessible at R, if
  -- a hypothetical public member of B would be a public member of N, or
  -- R is in a member/friend of N, and a hypothetical public member of B would be accessible in N, or
  -- R is in a member/friend of N's derived class P, and a hypothetical public member of B would be accessible in P, or
  -- there exists S such that B is a base of S accessible at R and S is a base of N accessible at R.

5.
If A base class B of class N is accessible at R, one can implicitly convert a pointer to N to a pointer to B at R.
A member m when named in class N is accessible at R, if
  -- m is public in N, or
  -- m is private in N, and R is in a member/friend of class N, or
  -- m is protected in N, and R is in a member/friend of class N, or
     m is protected in N, and R is in a member{/friend (until C++17)} of N's derived class P, and m is accessible in P, or
  -- there exists a base B of N that is accessible at R, and m when named in B is accessible at R.
(The class where a member is named is either explicitly specified using the :: operator or
 implicitly specified as the class of the object where the member is accessed with the . or -> operator.
 It is NOT necessarily the class where the member is declared.)

6.
When accessing a non-static member, a pointer to the object where the member is accessed
has to be implicitly convertible to a pointer to the class where the member is named
(when the :: operator is used to explicitly name a different class from the class of the object).

[class.protected]

1.
If a non-static member is protected in its naming class N, and is accessed at some class C (according to the rules above),
the type of the object expression where the member is accessed must be C or a class derived from C.

*/

struct Publ {
	virtual ~Publ() {}
};
struct Prot {
	virtual ~Prot() {}
};
struct Priv {
	virtual ~Priv() {}
};

struct N: public Publ, protected Prot, private Priv {
public:
	static int publ;
protected:
	static int prot;
private:
	static int priv;
public:
	void func(N *n);
	friend void fridN(N *n);
};

struct P: public N {
public:
	void func(N *n);
	friend void fridP(N *n);
};

int N::publ;
int N::prot;
int N::priv;

// When R is at any place:
void func(N *n) {
	Publ *base1 = n; // Rule 4.1
	n->publ = 0; // Rule 5.1
}

// When R is in a member of N:
void N::func(N *n) {
	Prot *base2 = n; // Rule 4.2
	Priv *base3 = n; // Rule 4.2
	n->prot = 0; // Rule 5.3 (first half)
	n->priv = 0; // Rule 5.2
}

// When R is in a friend of N:
void fridN(N *n) {
	Prot *base2 = n; // Rule 4.2
	Priv *base3 = n; // Rule 4.2
	n->prot = 0; // Rule 5.3 (first half)
	n->priv = 0; // Rule 5.2
}

// When R is in a member of N's derived class P:
void P::func(N *n) {
	Prot *base2 = n; // Rule 4.3
	n->prot = 0; // Rule 5.3 (second half)
	// The second line would be an error if "prot" is non-static, due to Rule [class.protected]/1.
}

// When R is in a friend of N's derived class P:
void fridP(N *n) {
	Prot *base2 = n; // Rule 4.3
	//n->prot = 0; // Rule 5.3 (second half)
	// The second line is an error since C++17 due to Defect Report CWG 1873.
	// But gcc accepts both lines and clang rejects both lines.
}

// ----------------------------------------------

int main() {
	return 0;
}
