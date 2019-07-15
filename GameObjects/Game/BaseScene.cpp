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
			transform->localPosition += vel;

			if (!transform->localPosition.InBounds(Vector3(5, 5, 5)))
				Destroy(*this);
		}
	};

	class BoxBehaviour : public GeometricObject
	{
	private:
		std::shared_ptr<DebugCameraWrapper> m_debugCamera;

		static Vector3 QuatToEuler(const Quaternion& quat)
		{
			float sqw = quat.w * quat.w;
			float sqx = quat.x * quat.x;
			float sqy = quat.y * quat.y;
			float sqz = quat.z * quat.z;

			float rotxrad = atan2f(2.0f * (quat.y * quat.z + quat.x * quat.w), (-sqx - sqy + sqz + sqw));
			float rotyrad = asinf(-2.0f * (quat.x * quat.z - quat.y * quat.w));
			float rotzrad = atan2f(2.0f * (quat.x * quat.y + quat.z * quat.w), (sqx - sqy - sqz + sqw));

			return Vector3(rotxrad, rotyrad, rotzrad);
		}

		static Vector3 ToEulerAngles(const Quaternion& q)
		{
			Vector3 angles;

			// roll (x-axis rotation)
			float sinr_cosp = +2.0f * (q.w * q.x + q.y * q.z);
			float cosr_cosp = +1.0f - 2.0f * (q.x * q.x + q.y * q.y);
			angles.x = atan2f(sinr_cosp, cosr_cosp);

			// pitch (y-axis rotation)
			float sinp = +2.0f * (q.w * q.y - q.z * q.x);
			if (fabsf(sinp) >= 1)
				angles.y = copysign(XM_PI / 2, sinp); // use 90 degrees if out of range
			else
				angles.y = asinf(sinp);

			// yaw (z-axis rotation)
			float siny_cosp = +2.0f * (q.w * q.z + q.x * q.y);
			float cosy_cosp = +1.0f - 2.0f * (q.y * q.y + q.z * q.z);
			angles.z = atan2f(siny_cosp, cosy_cosp);

			return angles;
		}

	public:
		float delta = 0;

	public:
		BoxBehaviour(const std::shared_ptr<DebugCameraWrapper>& debugCamera)
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
				auto sphere = std::make_shared<SphereBehaviour>();
				sphere->transform->localPosition = transform->localPosition;
				sphere->vel = Vector3::Transform(Vector3::Forward, transform->localRotation) * .1f;
				context << sphere;
			}
		}
	};

	auto box = std::make_shared<BoxBehaviour>(debugCamera);
	box->transform->localPosition = Vector3(0.f, 0.f, 5.f);
	context << box;
}
