#include "Keyboard.h"

// ���� Ű �ڵ带 �Է� �޾� �ش� Ű�� ���ȴ��� ���θ� �������ִ� �Լ�.
bool Keyboard::KeyIsPressed(unsigned char keycode) const noexcept
{
	return keystates[keycode];
}

// Ű Event ���ۿ� ����� ���� ������ Event�� ���� �������ִ� �Լ�.
std::optional<Keyboard::Event> Keyboard::ReadKey() noexcept
{
	if (keybuffer.size() > 0u)
	{
		Keyboard::Event e = keybuffer.front();
		keybuffer.pop();
		return e;
	}
	return {};
}

// Ű Event ���۰� ������� ���θ� �������ִ� �Լ�.
bool Keyboard::KeyIsEmpty() const noexcept
{
	return keybuffer.empty();
}

// Ű char ���ۿ� �ִ� ���� ������ char ���� ���� �������ִ� �Լ�.
std::optional<char> Keyboard::ReadChar() noexcept
{
	if (charbuffer.size() > 0u)
	{
		unsigned char charcode = charbuffer.front();
		charbuffer.pop();
		return charcode;
	}
	return {};
}

// Ű char ���۰� ������� ���θ� �������ִ� �Լ�.
bool Keyboard::CharIsEmpty() const noexcept
{
	return charbuffer.empty();
}

// Ű Event ���۸� ����ִ� �Լ�.
void Keyboard::FlushKey() noexcept
{
	keybuffer = std::queue<Event>();
}

// Ű char ���۸� ����ִ� �Լ�.
void Keyboard::FlushChar() noexcept
{
	charbuffer = std::queue<char>();
}

// Ű Event, char ���۸� ��� ����ִ� �Լ�.
void Keyboard::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

// autorepeatEnabled�� ���ִ� �Լ�.
void Keyboard::EnableAutorepeat() noexcept
{
	autorepeatEnabled = true;
}

// autorepeatEnabled�� ���ִ� �Լ�.
void Keyboard::DisableAutorepeat() noexcept
{
	autorepeatEnabled = false;
}

// autorepeatEnabled ���¸� �������ִ� �Լ�.
bool Keyboard::AutorepeatIsEnabled() const noexcept
{
	return autorepeatEnabled;
}

// �Է��� ���� Ű�� ���� Ű �ڵ带 �ϳ� �޾� Ű Event ���ۿ� �־��ִ� �Լ�.
void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{
	keystates[keycode] = true;                                                 // �ش� ���� Ű �ڵ带 �ε����� �ϴ� ��ġ�� ���� true�� �ٲ���.
	keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));    // Event Ÿ���� Press, �ش� ���� Ű �ڵ�� �Բ� Event ���ۿ� �־���.
	TrimBuffer(keybuffer);                                                     // ���� ������ ������ ���� ũ�⸦ ����� �ʵ��� ����.
}

// �Է��� �� ���� Ű �ڵ带 �ϳ� �޾� Ű Event ���ۿ� �־��ִ� �Լ�.
void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
	keystates[keycode] = false;                                                // �ش� ���� Ű �ڵ带 �ε����� �ϴ� ��ġ�� ���� false�� �ٲ���.
	keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));  // Event Ÿ���� Release, �ش� ���� Ű �ڵ�� �Բ� Event ���ۿ� �־���.
	TrimBuffer(keybuffer);                                                     // ���� ������ ������ ���� ũ�⸦ ����� �ʵ��� ����.
}

// �Է��� Ű�� char ���� �ϳ� �޾� char ���ۿ� �־��ִ� �Լ�.
void Keyboard::OnChar(char character) noexcept
{
	charbuffer.push(character);                                                // Ű char ���ۿ� char ���� �־���.
	TrimBuffer(charbuffer);                                                    // ���� ������ ������ ���� ũ�⸦ ����� �ʵ��� ����.
}

// ��� ���� Ű ���¸� �������ִ� �Լ�. ��� ���� Ű ���°� false�� ��.
void Keyboard::ClearState() noexcept
{
	keystates.reset();
}

// ������ ������ ���� ũ�⸦ �����ϵ��� ���۸� �������ִ� �Լ�.
template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	// �츮�� ������ ���� ũ�Ⱑ �� �� ���� ���ۿ� ����ִ� ���� ���� ������ �������� ������.
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}