#ifndef NUMCPP_CUDA_H_
#define NUMCPP_CUDA_H_

#include "base_array.h"
#include "array.h"

#include <cuda_runtime.h>

namespace numcpp {

template <typename T>
struct device_allocator
{
	static T *allocate(int size)
	{
		T *ptr = nullptr;
		cudaMalloc((void **)&ptr, size * sizeof(T));
		return ptr;
	}

	static void free(T *ptr)
	{
		cudaFree(ptr);
	}
};

template <typename T>
struct device_array_t : public base_array_t<T, device_allocator<T>>
{
public:
	device_array_t()
	{
		this->setEmpty();
	}

	device_array_t(int size0)
	{
		this->setSize(size0);
	}

	device_array_t(int size0, int size1)
	{
		this->setSize(size0, size1);
	}

	device_array_t(int size0, int size1, int size2)
	{
		this->setSize(size0, size1, size2);
	}

	~device_array_t()
	{
		this->free();
	}

private:
	// disable copy constructor, assign
	device_array_t(device_array_t &) { }
	const device_array_t &operator=(const device_array_t &) { return *this; }

public:
	// inherits move constructor
	device_array_t(device_array_t &&other) : 
		base_array_t<T, device_allocator<T>>(std::move(other))
	{
	}

	// inherits move assign
	const device_array_t &operator=(device_array_t &&other)
	{
		base_array_t<T, device_allocator<T>>::operator=(std::move(other));
		return *this;
	}

	// Convert from host array
	explicit device_array_t(const array_t<T> &array_h)
	{
		host_to_device(*this, array_h);
	}
};

template <typename T>
void host_to_device(device_array_t<T> &dst_d, const array_t<T> &src)
{
	dst_d.setSize(src.ndims(), src.shape());

	cudaMemcpy(dst_d, src, dst_d.size() * sizeof(T), cudaMemcpyHostToDevice);
}

template <typename T>
void device_to_host(array_t<T> &dst, const device_array_t<T> &src_d)
{
	dst.setSize(src_d.ndims(), src_d.shape());

	cudaMemcpy(dst, src_d, dst.size() * sizeof(T), cudaMemcpyDeviceToHost);
}

} // namespace numcpp

#endif // NUMCPP_CUDA_H_