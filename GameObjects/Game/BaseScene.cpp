#include "pch.h"
#include "BaseScene.h"
#include "SceneCommons.h"
#include <Framework/SceneManager.h>
#include <Framework/DebugTools/DebugCameraWrapper.h>
#include <Framework/DebugTools/GridFloorWrapper.h>
#include <Utilities/Input.h>
#include <Utilities/Random.h>
#include <Utilities/picojson.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

// ê∂ê¨
void BaseScene::Build(GameContext& context)
{
	auto debugCamera  = std::make_shared<DebugCameraWrapper>();
	context << debugCamera;

	auto gridFloor = std::make_shared<GridFloorWrapper>();
	context << gridFloor;

	class SphereBehaviour : public GeometricObject
	{
	public:
		Vector3 vel;

	public:
		SphereBehaviour()
			: GeometricObject([](GameContext& ctx) { return GeometricPrimitive::CreateSphere(ctx.GetDR().GetD3DDeviceContext()); }, Vector4(Colors::Gold))
		{
		}

		void Update(GameContext& context)
		{
			transform->LocalPosition += vel;

			if (!transform->LocalPosition.InBounds(Vector3(5, 5, 5)))
				Destroy(*this);
		}
	};

	class BoxBehaviour : public GeometricObject
	{
	public:
		float delta = 0;

	public:
		BoxBehaviour()
			: GeometricObject([](GameContext& ctx) { return GeometricPrimitive::CreateCube(ctx.GetDR().GetD3DDeviceContext()); })
		{
		}

		void Update(GameContext& context)
		{
			if (Input::GetKeyDown(Keyboard::Keys::Space))
			{
				auto sphere = std::make_shared<SphereBehaviour>();
				sphere->transform->LocalPosition = transform->LocalPosition;
				sphere->vel = Vector3(Random::Range(-1.f, 1.f), 0, Random::Range(0.f, -1.f)) * .2f;
				context << sphere;
			}
		}
	};

	auto box = std::make_shared<BoxBehaviour>();
	box->transform->LocalPosition = Vector3(0.f, 0.f, 5.f);
	context << box;
}
