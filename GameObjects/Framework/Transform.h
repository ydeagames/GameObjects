#pragma once

// Transformクラス
class Transform
{
public:
	// コンストラクタ
	Transform::Transform()
		: parent(nullptr)
		, localPosition(DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f))
		, localRotation(DirectX::SimpleMath::Vector3(0.f, 0.f, 0.f))
		, localScale(DirectX::SimpleMath::Vector3(1.f, 1.f, 1.f))
	{
	}

public:
	// ローカル位置
	DirectX::SimpleMath::Vector3 localPosition;
	// ローカル回転
	DirectX::SimpleMath::Quaternion localRotation;
	// ローカルスケール
	DirectX::SimpleMath::Vector3 localScale;

	struct
	{
		Transform* transform;
		void operator=(const DirectX::SimpleMath::Vector3& value)
		{
			if (transform->parent)
				transform->localPosition = DirectX::SimpleMath::Vector3::Transform(value, transform->parent->GetMatrix());
			else
				transform->localPosition = value;
		}
		operator DirectX::SimpleMath::Vector3()
		{
			if (transform->parent)
				return DirectX::SimpleMath::Vector3::Transform(transform->localPosition, transform->parent->GetMatrix().Invert());
			else
				return transform->localPosition;
		}
	} position{ this };

	struct
	{
		Transform* transform;
		void operator=(const DirectX::SimpleMath::Quaternion& value)
		{
			if (transform->parent)
				transform->localRotation = value * DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(transform->parent->GetMatrix());
			else
				transform->localRotation = value;
		}
		operator DirectX::SimpleMath::Quaternion()
		{
			if (transform->parent)
				return transform->localRotation * DirectX::SimpleMath::Quaternion::CreateFromRotationMatrix(transform->parent->GetMatrix().Invert());
			else
				return transform->localRotation;
		}
	} rotation{ this };

	struct
	{
		Transform* transform;
		void operator=(const DirectX::SimpleMath::Vector3& value)
		{
			if (transform->parent)
				transform->localScale = DirectX::SimpleMath::Vector3::Transform(value, transform->parent->GetMatrix());
			else
				transform->localScale = value;
		}
		operator DirectX::SimpleMath::Vector3()
		{
			if (transform->parent)
				return DirectX::SimpleMath::Vector3::Transform(transform->localScale, transform->parent->GetMatrix().Invert());
			else
				return transform->localScale;
		}
	} lossyScale{ this };

	const Transform* parent;
	void sparent(const Transform* value)
	{
		DirectX::SimpleMath::Vector3 worldPosition = position;
		DirectX::SimpleMath::Quaternion worldRotation = rotation;
		DirectX::SimpleMath::Vector3 worldScale = lossyScale;
		parent = value;
		position = worldPosition;
		rotation = worldRotation;
		lossyScale = worldScale;
	}

	// 行列取得
	DirectX::SimpleMath::Matrix GetMatrix() const
	{
		return
			DirectX::SimpleMath::Matrix::CreateTranslation(localPosition) *
			DirectX::SimpleMath::Matrix::CreateFromQuaternion(localRotation) *
			DirectX::SimpleMath::Matrix::CreateScale(localScale) *
			(parent != nullptr ? parent->GetMatrix() : DirectX::SimpleMath::Matrix::Identity);
	}
};