/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkInvertProcess.h>

#include <dtkLog>

#include <itkImage.h>
#include <itkInvertIntensityImageFilter.h>
#include <itkCommand.h>

#include <medAbstractImageData.h>
#include <medAbstractDataFactory.h>


medItkInvertProcess::medItkInvertProcess(QObject *parent)
    : medAbstractInvertFilterProcess(parent)

{
    m_filter = nullptr;
}

medItkInvertProcess::~medItkInvertProcess()
{

}

QString medItkInvertProcess::caption() const
{
    return "Invert image";
}

QString medItkInvertProcess::description() const
{
    return "Use ITK InvertIntensityImageFilter to invert image intensities.";
}

medAbstractJob::medJobExitStatus medItkInvertProcess::run()
{
    medAbstractJob::medJobExitStatus jobExitSatus = medAbstractJob::MED_JOB_EXIT_FAILURE;

    if(this->input())
    {
        QString id =  this->input()->identifier();

        if ( id == "itkDataImageChar3" )
        {
            jobExitSatus = this->_run<char>();
        }
        else if ( id == "itkDataImageUChar3" )
        {
            jobExitSatus = this->_run<unsigned char>();
        }
        else if ( id == "itkDataImageShort3" )
        {
            jobExitSatus = this->_run<short>();
        }
        else if ( id == "itkDataImageUShort3" )
        {
            jobExitSatus = this->_run<unsigned short>();
        }
        else if ( id == "itkDataImageInt3" )
        {
            jobExitSatus = this->_run<int>();
        }
        else if ( id == "itkDataImageUInt3" )
        {
            jobExitSatus = this->_run<unsigned int>();
        }
        else if ( id == "itkDataImageLong3" )
        {
            jobExitSatus = this->_run<long>();
        }
        else if ( id== "itkDataImageULong3" )
        {
            jobExitSatus = this->_run<unsigned long>();
        }
        else if ( id == "itkDataImageFloat3" )
        {
            jobExitSatus = this->_run<float>();
        }
        else if ( id == "itkDataImageDouble3" )
        {
            jobExitSatus = this->_run<double>();
        }
    }
    return jobExitSatus;
}

template <class inputType>
medAbstractJob::medJobExitStatus medItkInvertProcess::_run()
{
    typedef itk::Image<inputType, 3> ImageType;

    typename ImageType::Pointer in1 = dynamic_cast<ImageType *>((itk::Object*)(this->input()->data()));

    if(in1.IsNotNull())
    {
        typedef itk::InvertIntensityImageFilter<ImageType, ImageType> FilterType;
        typename FilterType::Pointer filter = FilterType::New();
        m_filter = filter;

        filter->SetInput(in1);

        itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
        callback->SetClientData((void*)this);
        callback->SetCallback(medItkInvertProcess::eventCallback);
        filter->AddObserver(itk::ProgressEvent(), callback);

        try
        {
            filter->Update();
        }
        catch(itk::ProcessAborted &)
        {
            return medAbstractJob::MED_JOB_EXIT_CANCELLED;
        }

        medAbstractImageData *out= qobject_cast<medAbstractImageData *>(medAbstractDataFactory::instance()->create(this->input()->identifier()));
        out->setData(filter->GetOutput());
        this->setOutput(out);
        return medAbstractJob::MED_JOB_EXIT_SUCCESS;
    }
    return medAbstractJob::MED_JOB_EXIT_FAILURE;


}

void medItkInvertProcess::cancel()
{
    if(this->isRunning() && m_filter.IsNotNull())
    {
        m_filter->AbortGenerateDataOn();
    }
}
