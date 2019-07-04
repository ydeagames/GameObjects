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

// ¶¬
void BaseScene::Build(GameContext& context)
{
	auto debugCamera  = GameObject::Create();
	debugCamera->AddComponent<DebugCameraWrapper>();
	context << debugCamera;

	auto gridFloor = GameObject::Create();
	gridFloor->AddComponent<GridFloorWrapper>();
	context << gridFloor;

	class BoxBehaviour : public Component
	{
		void Update(GameContext& context)
		{
			gameObject->transform->LocalPosition = Vector3::Transform(Vector3::Right, Matrix::CreateRotationY(float(context.GetTimer().GetTotalSeconds()))) * 4;
		}
	};
	auto box = GameObject::Create();
	box->AddComponent<GeometricObject>([](GameContext& ctx) { return GeometricPrimitive::CreateCube(ctx.GetDR().GetD3DDeviceContext()); });
	box->AddComponent<BoxBehaviour>();
	//box->transform->SetPosition(Vector3(2, 0, 0));
	context << box;

	class SphereBehaviour : public Component
	{
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
			if (Input::GetKey(Keyboard::Keys::Q) || Input::GetKey(Keyboard::Keys::LeftShift))
				input.y -= 1;
			if (Input::GetKey(Keyboard::Keys::E) || Input::GetKey(Keyboard::Keys::Space))
				input.y += 1;

			Vector3 s_, t_;
			Quaternion rotation;
			context.GetCamera().view.Decompose(s_, rotation, t_);
			rotation.Inverse(rotation);

			auto force = Vector3::Transform(input, rotation);
			gameObject->transform->LocalPosition += force;
		}
	};
	auto sphere = GameObject::Create();
	sphere->AddComponent<GeometricObject>([](GameContext& ctx) { return GeometricPrimitive::CreateSphere(ctx.GetDR().GetD3DDeviceContext()); });
	sphere->AddComponent<SphereBehaviour>();
	sphere->transform->SetPosition(Vector3(-1, 0, 3));
	context << sphere;
}
