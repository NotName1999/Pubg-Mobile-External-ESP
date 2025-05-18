#pragma once
#include<Windows.h>
#include<iostream>
#include<string>
#include<string>
#include<vector>
#include"Tools.h"
#include "Data.h"
#include <d3d9types.h>
using namespace std;
struct ActorsEncryption {
	uint32_t Enc_1; //0x0340 
	uint32_t Enc_2; //0x0344 
	uint32_t Enc_3; //0x0348 
	uint32_t Enc_4; //0x034C 
};
struct Encryption_Chunk {
	uint32_t val_1, val_2, val_3, val_4;
};
struct ObjectName
{
	char Data[64];
};
struct VECTOR2
{
	FLOAT X;
	FLOAT Y;
};

struct VECTOR3
{


	float X;
	float Y;
	float Z;

	float  Size()
	{
		return sqrt((this->X * this->X) + (this->Y * this->Y) + (this->Z * this->Z));
	}


	VECTOR3 FVector(float _X, float _Y, float _Z)
	{
		VECTOR3 emmm;
		emmm.X = _X;
		emmm.Y = _Y;
		emmm.Z = _Z;
		return emmm;
	}

	VECTOR3  operator+ (const VECTOR3& A)
	{
		return FVector(this->X + A.X, this->Y + A.Y, this->Z + A.Z);
	}

	VECTOR3 operator+ (const float A)
	{
		return FVector(this->X + A, this->Y + A, this->Z + A);
	}

	VECTOR3  operator+= (const VECTOR3& A)
	{
		this->X += A.X;
		this->Y += A.Y;
		this->Z += A.Z;
		return *this;
	}

	VECTOR3  operator+= (const float A)
	{
		this->X += A;
		this->Y += A;
		this->Z += A;
		return *this;
	}

	VECTOR3  operator- (const VECTOR3& A)
	{
		return FVector(this->X - A.X, this->Y - A.Y, this->Z - A.Z);
	}

	VECTOR3  operator- (const float A)
	{
		return FVector(this->X - A, this->Y - A, this->Z - A);
	}

	VECTOR3 operator-= (const VECTOR3& A)
	{
		this->X -= A.X;
		this->Y -= A.Y;
		this->Z -= A.Z;
		return *this;
	}

	VECTOR3 operator-= (const float A)
	{
		this->X -= A;
		this->Y -= A;
		this->Z -= A;
		return *this;
	}

	VECTOR3  operator* (const VECTOR3& A)
	{
		return FVector(this->X * A.X, this->Y * A.Y, this->Z * A.Z);
	}

	VECTOR3  operator* (const float A)
	{
		return FVector(this->X * A, this->Y * A, this->Z * A);
	}

	VECTOR3  operator*= (const VECTOR3& A)
	{
		this->X *= A.X;
		this->Y *= A.Y;
		this->Z *= A.Z;
		return *this;
	}

	VECTOR3 operator*= (const float A)
	{
		this->X *= A;
		this->Y *= A;
		this->Z *= A;
		return *this;
	}

	VECTOR3  operator/ (const VECTOR3& A)
	{
		return FVector(this->X / A.X, this->Y / A.Y, this->Z / A.Z);
	}

	VECTOR3 operator/ (const float A)
	{
		return FVector(this->X / A, this->Y / A, this->Z / A);
	}

	VECTOR3  operator/= (const VECTOR3& A)
	{
		this->X /= A.X;
		this->Y /= A.Y;
		this->Z /= A.Z;
		return *this;
	}

	VECTOR3 operator/= (const float A)
	{
		this->X /= A;
		this->Y /= A;
		this->Z /= A;
		return *this;
	}

	bool  operator== (const VECTOR3& A)
	{
		if (this->X == A.X
			&& this->Y == A.Y
			&& this->Z == A.Z)
			return true;

		return false;
	}

	bool operator!= (const VECTOR3& A)
	{
		if (this->X != A.X
			|| this->Y != A.Y
			|| this->Z != A.Z)
			return true;

		return false;
	}
	float Distance(VECTOR3 v)
	{
		return float(sqrtf(powf(v.X - X, 2.0) + powf(v.Y - Y, 2.0) + powf(v.Z - Z, 2.0)));
	}
};


struct VECTOR4
{
	FLOAT X;
	FLOAT Y;
	FLOAT Z;
	FLOAT W;
};

uint32_t DecryptActorsArrayZakaria(uint32_t uLevel, int Actors_Offset, int EncryptedActors_Offset)
{
	if (uLevel < 0x10000000)
		return 0;

	if (ReadMemoryEx<uint32_t>(uLevel + Actors_Offset) > 0)
		return uLevel + Actors_Offset;
	else
	{
		std::cout << "Try Find Next Decryption Address 1" << std::endl;

	}

	if (ReadMemoryEx<uint32_t>(uLevel + EncryptedActors_Offset) > 0)
		return uLevel + EncryptedActors_Offset;
	else
	{
		std::cout << "Try Find Next Decryption Address 2" << std::endl;

	}
	auto Encryption = ReadMemoryEx<ActorsEncryption>(uLevel + EncryptedActors_Offset + 0x0C);

	if (Encryption.Enc_1 > 0)
	{
		auto Enc = ReadMemoryEx<Encryption_Chunk>(Encryption.Enc_1 + 0x80);

		return ((ReadMemoryEx<unsigned __int8>(Encryption.Enc_1 + Enc.val_1))
			| ((ReadMemoryEx<unsigned __int8>(Encryption.Enc_1 + Enc.val_2)) << 8)
			| ((ReadMemoryEx<unsigned __int8>(Encryption.Enc_1 + Enc.val_3)) << 16)
			| ((ReadMemoryEx<unsigned __int8>(Encryption.Enc_1 + Enc.val_4)) << 24));
	} //ok
	else if (Encryption.Enc_2 > 0)
	{
		auto Encrypted_Actors = ReadMemoryEx<uint32_t>(Encryption.Enc_2);
		if (Encrypted_Actors > 0)
		{
			return (Encrypted_Actors - 0x4000000) & 0xFF000000
				| (Encrypted_Actors + 0xFC0000) & 0xFF0000
				| ((unsigned __int16)Encrypted_Actors - 0x400) & 0xFF00
				| (unsigned __int8)(Encrypted_Actors - 4);
		}
	} //ok
	else if (Encryption.Enc_3 > 0)
	{
		auto Encrypted_Actors = ReadMemoryEx<uint32_t>(Encryption.Enc_3);
		if (Encrypted_Actors > 0)
			return _rotr(Encrypted_Actors, 0x18);
	}
	else if (Encryption.Enc_4 > 0)
	{
		auto Encrypted_Actors = ReadMemoryEx<uint32_t>(Encryption.Enc_4);
		if (Encrypted_Actors > 0)
			return Encrypted_Actors ^ 0xCDCD00;
	}
	else
	{
		std::cout << "Cant Find Decryption Address" << std::endl;
	}
	return 0;
}




string GetEntities(LONG GNames, LONG Id)
{
	DWORD GName = ReadMemoryEx<DWORD>(GNames);
	DWORD NamePtr = ReadMemoryEx<DWORD>(GName + int(Id / 16384) * 0x4);
	DWORD Name = ReadMemoryEx<DWORD>(NamePtr + int(Id % 16384) * 0x4);
	ObjectName pBuffer = ReadMemoryEx<ObjectName>(Name + 0x8);
	return string(pBuffer.Data);
}
BOOL
IsPlayer(string str)
{
	if (str.find(("BP_PlayerPawn")) != std::string::npos ||
		str.find(("PlayerCharacter")) != std::string::npos ||
		str.find(("BP_Plane_Character_C")) != std::string::npos ||
		str.find(("BP_PlayerState")) != std::string::npos ||

		str.find(("PlanET_FakePlayer")) != std::string::npos ||//new
		str.find(("PlayerPawn_Infec")) != std::string::npos ||//new
		str.find(("Bp_XTPlayerState_C")) != std::string::npos ||
		str.find(("BP_CharacterModelTaget_C")) != std::string::npos)
	{
		return true;
	}
	return FALSE;
}


BOOL WorldToScreen(VECTOR3 Position, VECTOR3& Screen, D3DMATRIX ViewMatrix)
{
	FLOAT ScreenW = (ViewMatrix._14 * Position.X) + (ViewMatrix._24 * Position.Y) + (ViewMatrix._34 * Position.Z + ViewMatrix._44);

	if (ScreenW < 0.0001f) return FALSE;  // KHÔNG được bỏ dòng này

	ScreenW = 1.f / ScreenW;

	FLOAT SightX = (Process::Width / 2);  // Process::Width
	FLOAT SightY = (Process::Height / 2);   // Process::Height

	Screen.X = SightX + (ViewMatrix._11 * Position.X + ViewMatrix._21 * Position.Y + ViewMatrix._31 * Position.Z + ViewMatrix._41) * ScreenW * SightX;
	Screen.Y = SightY - (ViewMatrix._12 * Position.X + ViewMatrix._22 * Position.Y + ViewMatrix._32 * Position.Z + ViewMatrix._42) * ScreenW * SightY;

	return TRUE;
}


VECTOR3 Subtract(VECTOR3 Src, VECTOR3 Dst)
{
	VECTOR3 Result;
	Result.X = Src.X - Dst.X;
	Result.Y = Src.Y - Dst.Y;
	Result.Z = Src.Z - Dst.Z;

	return Result;
}
float Magnitude(VECTOR3 Vec)
{
	return sqrtf(Vec.X * Vec.X + Vec.Y * Vec.Y + Vec.Z * Vec.Z);
}
float GetDistance(VECTOR3 Src, VECTOR3 Dst)
{
	VECTOR3 Result = Subtract(Src, Dst);
	return Magnitude(Result);
}
