#include "pch.h"
#include "BaseScene.h"
#include "SceneCommons.h"
#include <Framework/SceneManager.h>
#include <Framework/DebugTools/DebugCameraWrapper.h>
#include <Framework/DebugTools/GridFloorWrapper.h>
#include <Framework/Scene.h>
#include <Utilities/Input.h>
#include <Utilities/Random.h>
#include <Utilities/picojson.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

// 生成
void BaseScene::Build(GameContext& context)
{
	auto debugCamera = std::make_shared<DebugCameraWrapper>();
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

			if (!BoundingBox(Vector3::Zero, Vector3(10, 5, 5)).Contains(transform->localPosition))
				vel.z *= -1;
			else if (!BoundingBox(Vector3::Zero, Vector3(5, 5, 10)).Contains(transform->localPosition))
				vel.x *= -1;
			else if (!BoundingBox(Vector3::Zero, Vector3(5, 5, 5)).Contains(transform->localPosition))
				vel *= -1;
		}
	};

	class BulletGenerator : public GameObject
	{
		float time = 0;

		void Update(GameContext& context)
		{
			time += float(context.GetTimer().GetElapsedSeconds());
			if (time > .5f)
			{
				time = 0;

				auto bullet = std::make_shared<BulletBehaviour>(transform);
				bullet->layer = Layer::LAYER_BALL;
				bullet->transform->localPosition = Vector3(Random::Range(-4.f, 4.f), 0, Random::Range(-4.f, 4.f));
				bullet->vel = Vector3::Transform(Vector3::Forward, Matrix::CreateFromAxisAngle(Vector3::UnitY, Random::Range(0.f, XM_2PI))) * .1f;
				context << bullet;
			}
		}
	};

	auto gen = std::make_shared<BulletGenerator>();
	context << gen;

	class CollisionController : public GameObject
	{
		void Update(GameContext& context)
		{
			auto& scene = context.GetScene();
			std::vector<std::pair<std::list<std::shared_ptr<GameObject>>&, std::list<std::shared_ptr<GameObject>>&>> collisionMatrix =
			{
				{ scene.gameObjects[Layer::LAYER_BALL], scene.gameObjects[Layer::LAYER_BALL] },
				{ scene.gameObjects[Layer::LAYER_PLAYER], scene.gameObjects[Layer::LAYER_BALL] },
			};

			for (auto& col : collisionMatrix)
				for (auto& obj1i : col.first)
					for (auto& obj2i : col.second)
					{
						float radius = .8f;
						if (obj1i < obj2i && Vector3::DistanceSquared(obj1i->transform->localPosition, obj2i->transform->localPosition) < (radius + radius) * (radius + radius))
						{
							auto obj1 = std::dynamic_pointer_cast<BulletBehaviour>(obj1i);
							auto obj2 = std::dynamic_pointer_cast<BulletBehaviour>(obj2i);
							if (obj1 && obj2)
							{
								// 位置の差のベクトルに対して垂直なベクトルと水平な方向に速度ベクトルを分解
								auto subpos = obj2->transform->localPosition -obj1->transform->localPosition;	// 位置の差
								auto subvel = obj2->vel - obj1->vel;	// 速度の差
								auto subposNorm = subpos;	// 位置の差単位ベクトル
								auto subvelNorm = subvel;	// 速度の差単位ベクトル
								subposNorm.Normalize();
								subvelNorm.Normalize();

								// 先生のArrow.cpp
								auto rotAngle = std::acos(subposNorm.Dot(subvelNorm));
								auto rotAxis = subposNorm.Cross(subvelNorm);
								if (rotAxis.Length() == 0.0f) rotAxis = Vector3(0.0f, 0.0f, 1.0f);

								// 90度回転して垂直なベクトル軸
								auto subposVerticalNorm = Vector3::Transform(subposNorm, Quaternion::CreateFromAxisAngle(rotAxis, XMConvertToRadians(90)));
								auto subvelLength = subvel.Length();
								// sin cosで長さを加味
								auto subvelVertical = subposVerticalNorm * subvelLength * std::sin(rotAngle);
								auto subvelHorizontal = subposNorm * subvelLength * std::cos(rotAngle);

								// 速度に加算
								auto w = 2;// obj1->m_objectWeight + obj2->m_objectWeight;
								auto w1 = 1;// obj1->m_objectWeight / w;
								auto w2 = 1;// obj2->m_objectWeight / w;
								obj1->vel = -(subvelHorizontal * -.5f + subvelVertical * w1);
								obj2->vel = +(subvelHorizontal * -.5f + subvelVertical * w2);
								//obj1->m_objectPos += obj1->m_objectVel;
								//obj2->m_objectPos += obj2->m_objectVel;
								auto sradius = radius + radius;
								auto subradius = subpos.Length() - sradius;
								auto subadd = subposNorm * subradius * .55f;
								obj1->transform->position += +subadd;
								obj2->transform->position += -subadd;
							}
						}
					}
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
				auto bounding = BoundingSphere(transform->localPosition, 2.5f);
				for (auto& layer : context.GetScene().gameObjects)
					for (auto& obj : layer)
						if (std::dynamic_pointer_cast<BulletBehaviour>(obj) && bounding.Contains(obj->transform->localPosition))
							Destroy(*obj);
			}
		}
	};

	auto player = std::make_shared<PlayerBehaviour>(debugCamera);
	player->layer = Layer::LAYER_PLAYER;
	player->transform->localPosition = Vector3(0.f, 0.f, 5.f);
	context << player;

	class CircleBehaviour : public GeometricObject
	{
		std::shared_ptr<Transform> m_playerTransform;

	public:
		CircleBehaviour(const std::shared_ptr<Transform>& playerTransform)
			: GeometricObject([](GameContext& ctx) { return GeometricPrimitive::CreateCylinder(ctx.GetDR().GetD3DDeviceContext(), 0.01f, 5); }, Vector4(Color(1, 1, 1, .2f)))
			, m_playerTransform(playerTransform)
		{
			transform = playerTransform;
		}
	};

	auto circle = std::make_shared<CircleBehaviour>(player->transform);
	context << circle;
}
