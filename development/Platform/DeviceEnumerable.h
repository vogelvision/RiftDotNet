#pragma once

#include <OVR_Device.h>

#include "RiftDotNet.h"
#include "TypedDeviceHandle.h"
#include "DeviceEnumerator.h"

using namespace System;
using namespace System::Collections::Generic;




namespace RiftDotNet
{
	namespace Platform
	{
		generic <typename TDevice, typename TInfo>
		where TDevice : IDevice
		where TInfo : IDeviceInfo
		public ref class DeviceEnumerable
			: public IEnumerable<TypedDeviceHandle<TDevice,TInfo>^>
		{
		public:

			DeviceEnumerable(const OVR::DeviceEnumerator<>& native)
			{
				_native = new OVR::DeviceEnumerator<>(native);
			}

			~DeviceEnumerable()
			{
				delete _native;
				_native = nullptr;
			}

			virtual IEnumerator<TypedDeviceHandle<TDevice,TInfo>^>^ GetEnumerator()
			{
				return gcnew Enumerator(new OVR::DeviceEnumerator<>(*_native));
			}

			virtual System::Collections::IEnumerator^ GetEnumerator2() = System::Collections::IEnumerable::GetEnumerator
			{
				return GetEnumerator();
			}

		private:

			ref class Enumerator sealed
				: public IEnumerator<TypedDeviceHandle<TDevice,TInfo>^>
			{
			public:

				Enumerator(OVR::DeviceEnumerator<>* native)
				{
					_native = native;
				}

				~Enumerator()
				{
					delete _native;
					_native = nullptr;
				}

				property TypedDeviceHandle<TDevice,TInfo>^ Current { 
					virtual TypedDeviceHandle<TDevice,TInfo>^ get() {
						auto type = (DeviceType)_native->GetType();
						if (type == DeviceType::None)
						{
							throw gcnew InvalidOperationException();
						}

						// It's important that we create a copy of the *current* state of the
						// enumeration, otherwise Next() is going to influence 
						return gcnew TypedDeviceHandle<TDevice,TInfo>(new OVR::DeviceHandle(*_native));
					}
				};

				property Object^ CurrentBase { 
					virtual Object^ get() = System::Collections::IEnumerator::Current::get { 
						return Current;
					} 
				};

				virtual bool MoveNext() { 
					return _native->Next();
				}

				virtual void Reset() { 
					_native->Clear();
				}

			private:

				OVR::DeviceEnumerator<>* _native;
			};

		private:

			OVR::DeviceEnumerator<>* _native;
		};
	}
}