﻿#include "UiInputSlider.hpp"

#include "Maths/Visual/DriverSlide.hpp"
#include "Renderer/Renderer.hpp"
#include "Scenes/Scenes.hpp"
#include "Uis/Uis.hpp"

namespace acid
{
	static const Time CHANGE_TIME = Time::Seconds(0.1f);
	static const Time SLIDE_TIME = Time::Seconds(0.15f);
	static const float FONT_SIZE = 1.7f;
	static const float SCALE_NORMAL = 1.0f;
	static const float SCALE_SELECTED = 1.1f;

	UiInputSlider::UiInputSlider(UiObject *parent, const std::string &prefix, const float &value,
	    const float &progressMin, const float &progressMax, const int32_t &roundTo,
	    const UiBound &rectangle, const Colour &primaryColour, const Colour &secondaryColour) :
		UiObject(parent, rectangle),
		m_background(std::make_unique<Gui>(this, UiBound::Maximum, Texture::Create("Guis/Button.png"), primaryColour)),
		m_slider(std::make_unique<Gui>(m_background.get(), UiBound(Vector2::Zero, UiReference::TopLeft, UiAspect::Position | UiAspect::Scale),
		    Texture::Create("Guis/Button.png"), secondaryColour)),
		m_text(std::make_unique<Text>(this, UiBound::Centre, FONT_SIZE, "",
			FontType::Create("Fonts/ProximaNova", "Regular"), Text::Justify::Centre, rectangle.GetDimensions().m_x, Colour::White)),
		m_soundClick(Sound("Sounds/Button1.ogg", Transform::Identity, Audio::Type::Effect, false, false, 0.9f)),
		m_prefix(prefix),
		m_updating(false),
		m_value(value),
		m_progressMin(progressMin),
		m_progressMax(progressMax),
		m_roundTo(roundTo),
		m_mouseOver(false),
		m_hasChange(false),
		m_timerChange(Timer(SLIDE_TIME)),
		m_onSlide(Delegate<void(UiInputSlider *, float)>())
	{
		m_background->GetRectangle().SetReference(UiReference::Centre);
		m_text->GetRectangle().SetReference(UiReference::Centre);
		SetValue(value);
	}

	void UiInputSlider::UpdateObject()
	{
		// Click updates.
		if (m_background->IsSelected() && GetAlpha() == 1.0f && Uis::Get()->WasDown(MouseButton::Left))
		{
			if (!m_updating && !m_soundClick.IsPlaying())
			{
				m_soundClick.SetPitch(Maths::Random(0.7f, 0.9f));
				m_soundClick.Play();
			}

			m_updating = true;
		}
		else if (!Uis::Get()->IsDown(MouseButton::Left))
		{
			if (m_updating && !m_soundClick.IsPlaying())
			{
				m_soundClick.SetPitch(Maths::Random(0.7f, 0.9f));
				m_soundClick.Play();
			}

			m_updating = false;
		}
		else if (m_updating)
		{
			float width = m_background->GetScreenDimension().m_x;
			float positionX = m_background->GetScreenPosition().m_x;
			float cursorX = Mouse::Get()->GetPositionX() - positionX;
			m_value = cursorX / width;
			m_value = std::clamp(m_value, 0.0f, 1.0f);

			m_hasChange = true;
			CancelEvent(MouseButton::Left);
		}

		// Updates the listener.
		if (m_hasChange && m_timerChange.IsPassedTime())
		{
			m_onSlide(this, m_value);

			UpdateText();
			m_hasChange = false;
			m_timerChange.ResetStartTime();
		}

		// Mouse over updates.
		if (m_background->IsSelected() && !m_mouseOver)
		{
			m_background->SetScaleDriver<DriverSlide>(m_background->GetScale(), SCALE_SELECTED, CHANGE_TIME);
			m_text->SetScaleDriver<DriverSlide>(m_text->GetScale(), FONT_SIZE * SCALE_SELECTED, CHANGE_TIME);
			m_mouseOver = true;
		}
		else if (!m_background->IsSelected() && !m_updating && m_mouseOver)
		{
			m_background->SetScaleDriver<DriverSlide>(m_background->GetScale(), SCALE_NORMAL, CHANGE_TIME);
			m_text->SetScaleDriver<DriverSlide>(m_text->GetScale(), FONT_SIZE * SCALE_NORMAL, CHANGE_TIME);
			m_mouseOver = false;
		}

		m_slider->GetRectangle().SetDimensions(Vector2(m_value, 1.0f));
	}

	void UiInputSlider::SetPrefix(const std::string &prefix)
	{
		m_prefix = prefix;
		UpdateText();
	}

	void UiInputSlider::SetValue(const float &value)
	{
		m_value = (value - m_progressMin) / (m_progressMax - m_progressMin);
		UpdateText();
	}

	void UiInputSlider::UpdateText()
	{
		float value = (m_value * (m_progressMax - m_progressMin)) + m_progressMin;
		value = Maths::RoundToPlace(value, m_roundTo);

		if (m_roundTo == 1)
		{
			m_text->SetString(m_prefix + String::To(static_cast<int>(value)));
			return;
		}

		m_text->SetString(m_prefix + String::To(value));
	}
}
