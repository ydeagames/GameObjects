#include "pch.h"
#include "LogoScene.h"
#include "SceneCommons.h"
#include "Framework/SceneManager.h"

void LogoScene::Build(GameContext& context)
{
	auto obj = GameObject::Create();
	auto text = obj->AddComponent<FontObject>(L"Resources/Fonts/logofont.spritefont", L"ƒƒSƒV[ƒ“");
	context << obj;

	class A : public Component
	{
		float lastTime;
		std::shared_ptr<FontObject> m_text;

	public:
		A(const std::shared_ptr<FontObject>& text)
			: m_text(text) {}

		void Initialize(GameContext& context)
		{
			lastTime = context.GetTimer().GetTotalSeconds();
		}

		void Update(GameContext& context)
		{
			float time = float(context.GetTimer().GetTotalSeconds()) - lastTime;

			wchar_t str[256];
			std::swprintf(str, L"ƒƒSƒV[ƒ“: %.2f•b", time);

			m_text->SetText(str);

			if (time > 3.f)
				context.GetSceneManager().LoadScene("TitleScene");
		}
	};
	auto obj1 = GameObject::Create();
	auto cmp = obj1->AddComponent<A>(text);
	context << obj1;
}
