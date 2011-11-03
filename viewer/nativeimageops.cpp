#include "nativeimageops.hpp"

boost::shared_ptr< isis::viewer::QProgressFeedback > isis::viewer::operation::NativeImageOps::m_ProgressFeedback;

isis::util::ivector4 isis::viewer::operation::NativeImageOps::getGlobalMin( const boost::shared_ptr< isis::viewer::ImageHolder > image, const util::ivector4 &startPos, const unsigned short &radius)
{
	switch (image->getISISImage()->getMajorTypeID() ) {
		case data::ValuePtr<int8_t>::staticID:
			return internGetMin<int8_t>(image, startPos, radius);
			break;
		case data::ValuePtr<uint8_t>::staticID:
			return internGetMin<uint8_t>(image, startPos, radius);
			break;
		case data::ValuePtr<int16_t>::staticID:
			return internGetMin<int16_t>(image, startPos, radius);
			break;
		case data::ValuePtr<uint16_t>::staticID:
			return internGetMin<uint16_t>(image, startPos, radius);
			break;
		case data::ValuePtr<int32_t>::staticID:
			return internGetMin<int32_t>(image, startPos, radius);
			break;
		case data::ValuePtr<uint32_t>::staticID:
			return internGetMin<uint32_t>(image, startPos, radius);
			break;
		case data::ValuePtr<int64_t>::staticID:
			return internGetMin<int64_t>(image, startPos, radius);
			break;
		case data::ValuePtr<uint64_t>::staticID:
			return internGetMin<uint64_t>(image, startPos, radius);
			break;
		case data::ValuePtr<float>::staticID:
			return internGetMin<float>(image, startPos, radius);
			break;
		case data::ValuePtr<double>::staticID:
			return internGetMin<double>(image, startPos, radius);
			break;
	}
}

isis::util::ivector4 isis::viewer::operation::NativeImageOps::getGlobalMax(const boost::shared_ptr< isis::viewer::ImageHolder > image, const util::ivector4 &startPos, const unsigned short &radius)
{
	switch (image->getISISImage()->getMajorTypeID() ) {
		case data::ValuePtr<int8_t>::staticID:
			return internGetMax<int8_t>(image, startPos, radius);
			break;
		case data::ValuePtr<uint8_t>::staticID:
			return internGetMax<uint8_t>(image, startPos, radius);
			break;
		case data::ValuePtr<int16_t>::staticID:
			return internGetMax<int16_t>(image, startPos, radius);
			break;
		case data::ValuePtr<uint16_t>::staticID:
			return internGetMax<uint16_t>(image, startPos, radius);
			break;
		case data::ValuePtr<int32_t>::staticID:
			return internGetMax<int32_t>(image, startPos, radius);
			break;
		case data::ValuePtr<uint32_t>::staticID:
			return internGetMax<uint32_t>(image, startPos, radius);
			break;
		case data::ValuePtr<int64_t>::staticID:
			return internGetMax<int64_t>(image, startPos, radius);
			break;
		case data::ValuePtr<uint64_t>::staticID:
			return internGetMax<uint64_t>(image, startPos, radius);
			break;
		case data::ValuePtr<float>::staticID:
			return internGetMax<float>(image, startPos, radius);
			break;
		case data::ValuePtr<double>::staticID:
			return internGetMax<double>(image, startPos, radius);
			break;
	}
}
void isis::viewer::operation::NativeImageOps::setProgressFeedBack(boost::shared_ptr< isis::viewer::QProgressFeedback > progressFeedback)
{
	m_ProgressFeedback = progressFeedback;
}
