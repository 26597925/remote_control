#pragma once

//������

class CConfig
{
public:
	CConfig(void);
	~CConfig(void);
    void CheckConfigFile();
	UINT GetPort();
	UINT GetMaxConn();
	UINT GetSound();
private:
	void WriteDefaultInfo();  //д��������Ϣ
};
