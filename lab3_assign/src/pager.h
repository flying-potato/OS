class Pager{
	public:
	Pager(){

	}

	virtual Frame* allocate_frame(Frame **frame_old );
};

class FIFO: public Pager{
	public:
	FIFO():Pager(){	}
};

