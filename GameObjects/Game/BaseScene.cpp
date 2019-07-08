#include "pch.h"
#include "BaseScene.h"
#include "SceneCommons.h"
#include <Framework/SceneManager.h>
#include <Framework/DebugTools/DebugCameraWrapper.h>
#include <Framework/DebugTools/GridFloorWrapper.h>
#include <Utilities/Input.h>
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
			transform->LocalPosition = Vector3::Transform(Vector3::Right, Matrix::CreateRotationY(float(context.GetTimer().GetTotalSeconds()) + delta)) * 4;
		}
	};
	for (int i = 0; i < 10; i++)
	{
		auto box = std::make_shared<BoxBehaviour>();
		box->delta = .2f * i;
		context << box;
	}

	class SphereBehaviour : public GeometricObject
	{
	public:
		SphereBehaviour()
			: GeometricObject([](GameContext& ctx) { return GeometricPrimitive::CreateSphere(ctx.GetDR().GetD3DDeviceContext()); })
		{
		}

		void Update(GameContext& context)
		{
			auto input = Vector3::Zero;
			if (Input::GetKey(Keyboard::Keys::A) || Input::GetKey(Keyboard::Keys::Left))
				input.x -= 1;
			if (Input::GetKey(Keyboard::Keys::D) || Input::GetKey(Keyboard::Keys::Right))
				input.x += 1;
			if (Input::GetKey(Keyboard::Keys::W) || Input::GetKey(Keyboard::Keys::Up))
				input.z -= 1;
			if (Input::GetKey(Keyboard::Keys::S) || Input::GetKey(Keyboard::Keys::Down))
				input.z += 1;

			Vector3 s_, t_;
			Quaternion rotation;
			context.GetCamera().view.Decompose(s_, rotation, t_);
			rotation.Inverse(rotation);

			input = Vector3::Transform(input, rotation);
			input.y = 0;
			input.Normalize();

			if (Input::GetKey(Keyboard::Keys::Q) || Input::GetKey(Keyboard::Keys::LeftShift))
				input.y -= 1;
			if (Input::GetKey(Keyboard::Keys::E) || Input::GetKey(Keyboard::Keys::Space))
				input.y += 1;

			input.Normalize();

			transform->LocalPosition += input * .2f;
		}
	};
	auto sphere = std::make_shared<SphereBehaviour>();
	sphere->transform->SetPosition(Vector3(-1, 0, 3));
	context << sphere;
}
