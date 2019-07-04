#include "pch.h"
#include "PlayScene.h"
#include "SceneCommons.h"
#include "Framework/SceneManager.h"
#include "Framework/DebugTools/DebugCameraWrapper.h"
#include "Framework/DebugTools/GridFloorWrapper.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void PlayScene::Build(GameContext& context)
{
	context.GetCamera().view = Matrix::CreateLookAt(Vector3(0, 5, 10), Vector3::Zero, Vector3::Up);

	auto camCtrl = GameObject::Create();
	camCtrl->AddComponent<DebugCameraWrapper>();
	camCtrl->AddComponent<GridFloorWrapper>();
	context << camCtrl;

	auto bitObj = GameObject::Create();
	auto bit = bitObj->AddComponent<GeometricObject>(
		[](GameContext& context) { return GeometricPrimitive::CreateTeapot(context.GetDR().GetD3DDeviceContext()); },
		Color(Colors::Blue)
		);
	bitObj->transform->LocalPosition += Vector3::Up * 2;
	context << bitObj;

	struct A : public Component
	{
		std::unique_ptr<PrimitiveBatch<VertexPositionColor>> m_batch;

		void Initialize(GameContext& ctx)
		{
			m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(ctx.GetDR().GetD3DDeviceContext());
		}

		void Render(GameContext& ctx)
		{
			Vector3 vecA = Vector3::Up * 3;
			Vector3 vecB = Vector3::Right * 3;
			Vector3 vecC = vecA.Cross(vecB);

			m_batch->Begin();
			m_batch->DrawLine(VertexPositionColor(Vector3::Zero, Colors::Blue), VertexPositionColor(vecA, Colors::Blue));
			m_batch->DrawLine(VertexPositionColor(Vector3::Zero, Colors::Red), VertexPositionColor(vecB, Colors::Red));
			m_batch->DrawLine(VertexPositionColor(Vector3::Zero, Colors::Green), VertexPositionColor(vecC, Colors::Green));
			m_batch->End();
		}
	};
	auto obj2 = GameObject::Create();
	auto cmp2 = obj2->AddComponent<A>();
	context << obj2;
}
