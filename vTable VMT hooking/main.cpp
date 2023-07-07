#include <iostream>
#include <Windows.h>
bool IsKeyDown(int virtualKeyCode)
{
	// Check the high-order bit to determine if the key is currently down
	return (GetAsyncKeyState(virtualKeyCode) & 0x8000) != 0;
}

class Entity
{
public:
	virtual void Talk() = 0;
	virtual void healthCheack() = 0;

	int health;
};

class Player : public Entity
{
public:
	virtual void Talk() override {
		printf("Hello from player 0x%p\n", this);
	}
	virtual void healthCheack() override {

		health = 100;
		printf("Health = %i\n", health);
	}
};

void (*Talk_orig)(void*);
void Talk_hook(void* _this)
{
	printf("Talked HOOKED 0x%p\n", _this);
	Talk_orig(_this);
}

static uint64_t hooked_vtable[4096];

int main()
{
	//EntBase
	Player* player = new Player;
	//Get Vtable from EntBase
	uint64_t* Vtable = *(uint64_t**)(player + 0x0);

	printf("Vtable = 0x%11x\n", Vtable);

	//get the function in the Vtable
	uint64_t function_address = Vtable[0];

	printf("Fiunction = 0x%11x\n", function_address);

	//declare function and prams
	void(*Talk_fn)(void*) = 0;
	Talk_fn = (decltype(Talk_fn))function_address;

	//call Vtable func
	Talk_fn(player);

	
	memcpy(hooked_vtable, Vtable, (sizeof(hooked_vtable) / sizeof(uint64_t))); // copys Vtable to are hooked_Vtable
	hooked_vtable[0] = (uint64_t)&Talk_hook;//changes the function to are hook functio  
	Talk_orig = (decltype(Talk_orig))Vtable[0];//saves orignal func 
	*(PVOID*)(player + 0x0) = hooked_vtable;//swap vTable with are vTable


	Vtable = *(uint64_t**)player;//Reads new vTable
	Talk_fn = (decltype(Talk_fn))Vtable[0];// get prams for are function in the table
	Talk_fn(player);// calls are fuction


	while (!IsKeyDown(VK_DELETE))
	{
		Sleep(100);
	}
	return 0;

}