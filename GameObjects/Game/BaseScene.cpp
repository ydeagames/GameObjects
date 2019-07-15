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

	class BulletBehaviour : public GeometricObject
	{
	public:
		Vector3 vel;
		std::shared_ptr<Transform> m_playerTransform;

	public:
		BulletBehaviour(const std::shared_ptr<Transform>& playerTransform)
			: GeometricObject([](GameContext& ctx) { return GeometricPrimitive::CreateSphere(ctx.GetDR().GetD3DDeviceContext()); }, Vector4(Colors::Gold))
			, m_playerTransform(playerTransform)
		{
		}

		void Update(GameContext& context)
		{
			transform->localPosition += vel;

			if (!BoundingSphere(m_playerTransform->localPosition, 5).Contains(transform->localPosition))
				Destroy(*this);
		}
	};

	class PlayerBehaviour : public GeometricObject
	{
	private:
		std::shared_ptr<DebugCameraWrapper> m_debugCamera;

	public:
		float delta = 0;

	public:
		PlayerBehaviour(const std::shared_ptr<DebugCameraWrapper>& debugCamera)
			: GeometricObject([](GameContext& ctx) { return GeometricPrimitive::CreateTeapot(ctx.GetDR().GetD3DDeviceContext()); })
			, m_debugCamera(debugCamera)
		{
		}

		void Update(GameContext& context)
		{
			m_debugCamera->transform->localPosition = transform->localPosition;

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

				//auto euler = QuatToEuler(rotation);
				//transform->LocalEulerAngles = euler;
				auto flat = Vector3::Transform(Vector3::Forward, rotation);
				flat.y = 0;
				auto flatRotation = Quaternion::CreateFromRotationMatrix(Matrix::CreateWorld(Vector3::Zero, flat, Vector3::Up));
				transform->localRotation = flatRotation;

				auto force = input * .1f;
				transform->localPosition += force;
			}

			if (Input::GetKeyDown(Keyboard::Keys::Space))
			{
				auto bullet = std::make_shared<BulletBehaviour>(transform);
				bullet->transform->localPosition = transform->localPosition;
				bullet->vel = Vector3::Transform(Vector3::Forward, transform->localRotation) * .1f;
				context << bullet;
			}
		}
	};

	auto player = std::make_shared<PlayerBehaviour>(debugCamera);
	player->transform->localPosition = Vector3(0.f, 0.f, 5.f);
	context << player;

	class CircleBehaviour : public GeometricObject
	{
		std::shared_ptr<Transform> m_playerTransform;

	public:
		CircleBehaviour(const std::shared_ptr<Transform>& playerTransform)
			: GeometricObject([](GameContext& ctx) { return GeometricPrimitive::CreateTorus(ctx.GetDR().GetD3DDeviceContext(), 10, 0.1f); }, Vector4(Colors::Red))
			, m_playerTransform(playerTransform)
		{
			transform = playerTransform;
		}
	};

	auto circle = std::make_shared<CircleBehaviour>(player->transform);
	context << circle;
}
