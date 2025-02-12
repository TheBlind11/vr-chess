#pragma once

class CHESS_API FLogger
{

public:
	// Funzione statica da chiamare per scrivere una riga di log
	static void Log(const FString& Message);

private:
	// Rendiamo il costruttore privato per evitare istanziazioni
	FLogger() {}
	~FLogger() {}

};