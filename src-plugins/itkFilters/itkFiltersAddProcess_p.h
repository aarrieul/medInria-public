/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractData.h>

#include <itkFiltersProcessBase_p.h>

#include <medMetaDataKeys.h>

#include <itkImage.h>
#include <itkCommand.h>
#include <itkAddImageFilter.h>

class itkFiltersAddProcess;

class itkFiltersAddProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersAddProcessPrivate(itkFiltersAddProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersAddProcessPrivate(const itkFiltersAddProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersAddProcessPrivate(void) {}
    
    double addValue;
    
    template <class PixelType> void update ( void )
    {        
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::AddImageFilter<ImageType, itk::Image<double, ImageType::ImageDimension>, ImageType> AddFilterType;
        typename AddFilterType::Pointer addFilter = AddFilterType::New();
        
        try
        {
            addFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
            addFilter->SetConstant ( addValue );

            callback = itk::CStyleCommand::New();
            callback->SetClientData ( ( void * ) this );
            callback->SetCallback ( itkFiltersProcessBasePrivate::eventCallback );

            addFilter->AddObserver ( itk::ProgressEvent(), callback );

            addFilter->Update();
            output->setData ( addFilter->GetOutput() );

            QString newSeriesDescription = input->metadata ( medMetaDataKeys::SeriesDescription.key() );
            newSeriesDescription += " add filter (" + QString::number(addValue) + ")";

            output->addMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );
        }
        catch( itk::ExceptionObject & err )
        {
            std::cerr << "ExceptionObject caught in itkFiltersAddProcess!" << std::endl;
            std::cerr << err << std::endl;
        }
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersAddProcess, itkFiltersProcessBase)


