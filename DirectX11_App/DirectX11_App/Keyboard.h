#pragma once
#include <queue>
#include <bitset>

// Keyboard : Ű���� �Է��� ó�����ִ� Ŭ����.
class Keyboard
{
	friend class Window;		// Window Ŭ������ private ����� ������ �� �ֵ��� ����.
public:
#pragma region Event
	class Event
	{
	public:
		enum class Type			// Ű �̺�Ʈ�� Ÿ��.
		{
			Press,				// Ű ����.
			Release,			// Ű ��.
			Invalid				// �ʱ�ȭ ��. �ƹ� �͵� �ƴ� ����.
		};
	private:
		Type type;
		unsigned char code;		// Ű Event�� �߻���Ų Ű �ڵ带 ��� �ֵ��� ��.
	public:
		Event()
			:
			type(Type::Invalid),
			code(0u)
		{}
		Event(Type type, unsigned char code) noexcept
			:
			type(type),
			code(code)
		{}
		bool IsPress() const noexcept
		{
			return type == Type::Press;
		}
		bool IsRelease() const noexcept
		{
			return type == Type::Release;
		}
		bool IsValid() const noexcept
		{
			return type != Type::Invalid;
		}
		unsigned char GetCode() const noexcept
		{
			return code;
		}
	};
#pragma endregion
public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;
#pragma region KeyEvent
	bool KeyIsPressed(unsigned char keycode) const noexcept;
	Event ReadKey() noexcept;
	bool KeyIsEmpty() const noexcept;
	void FlushKey() noexcept;
#pragma endregion
#pragma region CharEvent
	char ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept; // key, char�� �Ѵ� flush
#pragma endregion
#pragma region AutoRepeat
	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool AutorepeatIsEnabled() const noexcept;
#pragma endregion
private: // Window Ŭ������ ����� �Լ���.
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;
private:
	static constexpr unsigned int nKeys = 256u;       // ���� Ű �ڵ��� ����. 1����Ʈ 256��.
	static constexpr unsigned int bufferSize = 16u;   // Ű �̺�Ʈ�� �������� ������ �ִ� ũ��.
	bool autorepeatEnabled = false;                   // autorepeatEnabled üũ ����.
	std::bitset<nKeys> keystates;                     // �ش� ���� Ű�� ���ȴ��� ���θ� ������ bitset.
	std::queue<Event> keybuffer;                      // Ű Event�� ������ �� ����.
	std::queue<char> charbuffer;                      // Ű Event�� ���� char�� �������� ����.
};