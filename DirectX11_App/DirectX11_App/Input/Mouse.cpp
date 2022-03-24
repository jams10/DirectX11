#include "Mouse.h"
#include <Windows.h>

// ���콺 ��ǥ�� pair Ÿ������ �����ϴ� �Լ�.
std::pair<int, int> Mouse::GetPos() const noexcept
{
	return { x,y };
}

// ���콺 X ��ǥ�� �����ϴ� �Լ�.
int Mouse::GetPosX() const noexcept
{
	return x;
}

// ���콺 Y ��ǥ�� �����ϴ� �Լ�.
int Mouse::GetPosY() const noexcept
{
	return y;
}

// ���콺�� ������ ���� �ȿ� �ִ��� ���θ� �����ϴ� �Լ�.
bool Mouse::IsInWindow() const noexcept
{
	return isInWindow;
}

// ���콺 ���� ��ư�� ���ȴ��� ���θ� �����ϴ� �Լ�.
bool Mouse::LeftIsPressed() const noexcept
{
	return leftIsPressed;
}

// ���콺 ������ ��ư�� ���ȴ��� ���θ� �����ϴ� �Լ�.
bool Mouse::RightIsPressed() const noexcept
{
	return rightIsPressed;
}

// ���콺 Event ���ۿ��� ���� ������ Event �ϳ��� �������� �Լ�.
Mouse::Event Mouse::Read() noexcept
{
	if (buffer.size() > 0u)
	{
		Mouse::Event e = buffer.front();
		buffer.pop();
		return e;
	}
	else
	{
		return Mouse::Event();
	}
}

// ���콺 Event ���۸� ����ִ� �Լ�.
void Mouse::Flush() noexcept
{
	buffer = std::queue<Event>();
}

// ���콺�� ������ ���, ���콺 ��ǥ�� �����ϰ� ���콺 Event ���ۿ� Move Event�� �־��ִ� �Լ�.
void Mouse::OnMouseMove(int newx, int newy) noexcept
{
	x = newx;
	y = newy;

	buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
	TrimBuffer();
}

// ���콺�� ������ ���� ������ ���� ��츦 ó�����ִ� �Լ�.
void Mouse::OnMouseLeave() noexcept
{
	isInWindow = false;
	buffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
	TrimBuffer();
}

// ���콺�� ������ ���� ������ ���� ��츦 ó�����ִ� �Լ�.
void Mouse::OnMouseEnter() noexcept
{
	isInWindow = true;
	buffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
	TrimBuffer();
}

// ���콺 ���� ��ư�� ���� ��츦 ó�����ִ� �Լ�.
void Mouse::OnLeftPressed(int x, int y) noexcept
{
	leftIsPressed = true;

	buffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
	TrimBuffer();
}

// ���콺 ���� ��ư�� �� ��츦 ó�����ִ� �Լ�.
void Mouse::OnLeftReleased(int x, int y) noexcept
{
	leftIsPressed = false;

	buffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
	TrimBuffer();
}

// ���콺 ������ ��ư�� ���� ��츦 ó�����ִ� �Լ�.
void Mouse::OnRightPressed(int x, int y) noexcept
{
	rightIsPressed = true;

	buffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
	TrimBuffer();
}

// ���콺 ������ ��ư�� �� ��츦 ó�����ִ� �Լ�.
void Mouse::OnRightReleased(int x, int y) noexcept
{
	rightIsPressed = false;

	buffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
	TrimBuffer();
}

// ���콺 ���� �ø� ��츦 ó�����ִ� �Լ�.
void Mouse::OnWheelUp(int x, int y) noexcept
{
	buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
	TrimBuffer();
}

// ���콺 ���� ���� ��츦 ó�����ִ� �Լ�.
void Mouse::OnWheelDown(int x, int y) noexcept
{
	buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
	TrimBuffer();
}

// ������ ������ ���� ũ�⸦ �����ϵ��� ���۸� �������ִ� �Լ�.
void Mouse::TrimBuffer() noexcept
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}

// ���콺 �� �̺�Ʈ�� �������ִ� �Լ�.
void Mouse::OnWheelDelta(int x, int y, int delta) noexcept
{
	wheelDeltaCarry += delta;

	// ���밪�� 120�� �� ������ �� �̺�Ʈ ����. 
	while (wheelDeltaCarry >= WHEEL_DELTA)
	{
		wheelDeltaCarry -= WHEEL_DELTA;
		OnWheelUp(x, y);
	}
	while (wheelDeltaCarry <= -WHEEL_DELTA)
	{
		wheelDeltaCarry += WHEEL_DELTA;
		OnWheelDown(x, y);
	}
}