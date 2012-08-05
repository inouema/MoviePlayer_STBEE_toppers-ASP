/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2007-2011 by TAKAGI Nobuhisa
 * 
 *  �嵭����Ԥϡ��ʲ���(1)��(4)�ξ������������˸¤ꡤ�ܥ��եȥ���
 *  �����ܥ��եȥ���������Ѥ�����Τ�ޤࡥ�ʲ�Ʊ���ˤ���ѡ�ʣ������
 *  �ѡ������ۡʰʲ������ѤȸƤ֡ˤ��뤳�Ȥ�̵���ǵ������롥
 *  (1) �ܥ��եȥ������򥽡��������ɤη������Ѥ�����ˤϡ��嵭������
 *      ��ɽ�����������Ѿ�浪��Ӳ�����̵�ݾڵ��꤬�����Τޤޤη��ǥ���
 *      ����������˴ޤޤ�Ƥ��뤳�ȡ�
 *  (2) �ܥ��եȥ������򡤥饤�֥������ʤɡ�¾�Υ��եȥ�������ȯ�˻�
 *      �ѤǤ�����Ǻ����ۤ�����ˤϡ������ۤ�ȼ���ɥ�����ȡ�����
 *      �ԥޥ˥奢��ʤɡˤˡ��嵭�����ɽ�����������Ѿ�浪��Ӳ���
 *      ��̵�ݾڵ����Ǻܤ��뤳�ȡ�
 *  (3) �ܥ��եȥ������򡤵�����Ȥ߹���ʤɡ�¾�Υ��եȥ�������ȯ�˻�
 *      �ѤǤ��ʤ����Ǻ����ۤ�����ˤϡ����Τ����줫�ξ�����������
 *      �ȡ�
 *    (a) �����ۤ�ȼ���ɥ�����ȡ����Ѽԥޥ˥奢��ʤɡˤˡ��嵭����
 *        �ɽ�����������Ѿ�浪��Ӳ�����̵�ݾڵ����Ǻܤ��뤳�ȡ�
 *    (b) �����ۤη��֤��̤�������ˡ�ˤ�äơ�TOPPERS�ץ������Ȥ�
 *        ��𤹤뤳�ȡ�
 *  (4) �ܥ��եȥ����������Ѥˤ��ľ��Ū�ޤ��ϴ���Ū�������뤤���ʤ�»
 *      ������⡤�嵭����Ԥ����TOPPERS�ץ������Ȥ����դ��뤳�ȡ�
 *      �ޤ����ܥ��եȥ������Υ桼���ޤ��ϥ���ɥ桼������Τ����ʤ���
 *      ͳ�˴�Ť����ᤫ��⡤�嵭����Ԥ����TOPPERS�ץ������Ȥ�
 *      ���դ��뤳�ȡ�
 * 
 *  �ܥ��եȥ������ϡ�̵�ݾڤ��󶡤���Ƥ����ΤǤ��롥�嵭����Ԥ�
 *  ���TOPPERS�ץ������Ȥϡ��ܥ��եȥ������˴ؤ��ơ�����λ�����Ū
 *  ���Ф���Ŭ������ޤ�ơ������ʤ��ݾڤ�Ԥ�ʤ����ޤ����ܥ��եȥ���
 *  �������Ѥˤ��ľ��Ū�ޤ��ϴ���Ū�������������ʤ�»���˴ؤ��Ƥ⡤��
 *  ����Ǥ�����ʤ���
 * 
 */
#include <fstream>
#include <sstream>
#include "toppers/diagnostics.hpp"
#include "toppers/s_record.hpp"
#include "toppers/macro_processor.hpp"
#include "toppers/itronx/component.hpp"
#include "cfg.hpp"
#include <boost/spirit/include/classic.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

/*!
 *  \brief  ���֥�������ID�ֹ�γ��դ�
 *  \param[in]  api_map   �������˵��Ҥ��줿��ŪAPI����Ͽ��������ƥ�
 */
void assign_id( toppers::itronx::cfg1_out::static_api_map& api_map )
{
  using namespace toppers;
  using namespace toppers::itronx;

  std::string id_input_file( get_global< std::string >( "id-input-file" ) );
  if ( id_input_file.empty() )  // --id-input-file ���ץ���󤬻��ꤵ��Ƥ��ʤ����...
  {
    for ( cfg1_out::static_api_map::iterator iter( api_map.begin() ), last( api_map.end() );
          iter != last;
          ++iter )
    {
      static_api::assign_id( iter->second.begin(), iter->second.end() );
    }
  }
  else  // --id-input-file ���ץ���󤬻��ꤵ��Ƥ�����...
  {
    typedef std::map< std::string, std::pair< long, bool > > id_map_t;
    id_map_t id_map;
    std::ifstream ifs( id_input_file.c_str() );
    while ( ifs )
    {
      std::string linebuf;
      std::getline( ifs, linebuf );
      if ( ifs.bad() )
      {
        fatal( _( "I/O error" ) );
      }
      if ( linebuf.empty() || linebuf == "\r" )
      {
        break;
      }

      std::istringstream iss( linebuf );
      std::string name;
      iss >> name;
      if ( iss.fail() )
      {
        fatal( _( "id file `%1%\' is invalid" ), id_input_file );
      }

      long value;
      iss >> value;
      if ( iss.fail() )
      {
        fatal( _( "id file `%1%\' is invalid" ), id_input_file );
      }

      if ( id_map.find( name ) != id_map.end() )
      {
        fatal( _( "E_OBJ: `%1%\' is duplicated" ), name );
      }
      else
      {
        id_map[ name ] = std::make_pair( value, false );
      }
    }

    for ( cfg1_out::static_api_map::iterator iter( api_map.begin() ), last( api_map.end() );
          iter != last;
          ++iter )
    {
      for ( std::vector< static_api >::iterator iter2( iter->second.begin() ), last2( iter->second.end() );
            iter2 != last2;
            ++iter2 )
      {
        static_api::info const* info = iter2->get_info();
        if ( info->id_pos >= 0 )
        {
          std::string name( iter2->at( info->id_pos ).text );
          std::string symbol( iter2->at( info->id_pos ).symbol );
          if ( !info->slave && symbol[0] == '#' )
          {
            id_map_t::iterator hit( id_map.find( name ) );
            if ( hit != id_map.end() )
            {
              long id_value = hit->second.first;
              if ( id_value > 0 )
              {
                iter2->at( info->id_pos ).value = id_value;
                hit->second.second = true;
              }
            }
          }
        }
      }
      static_api::assign_id( iter->second.begin(), iter->second.end() );
    }

    for ( id_map_t::const_iterator iter( id_map.begin() ), last( id_map.end() ); iter != last; ++iter )  // �Ĥ�ʪ�������...
    {
      if ( !iter->second.second )
      {
        warning( _( "object identifier `%1%\' is not used" ), iter->first );
      }
    }
  }

  // --id-output-file ���ץ���󤬻��ꤵ��Ƥ�����
  std::string id_output_file( get_global< std::string >( "id-output-file" ) );
  if ( !id_output_file.empty() )
  {
    std::ofstream ofs( id_output_file.c_str() );
    for ( cfg1_out::static_api_map::iterator iter( api_map.begin() ), last( api_map.end() );
      iter != last;
      ++iter )
    {
      for ( std::vector< static_api >::const_iterator iter2( iter->second.begin() ), last2( iter->second.end() );
            iter2 != last2;
            ++iter2 )
      {
        static_api::info const* info = iter2->get_info();
        if ( info->id_pos >= 0 )
        {
          std::string name( iter2->at( info->id_pos ).text );
          std::string symbol( iter2->at( info->id_pos ).symbol );
          if ( !info->slave && symbol[0] == '#' )
          {
            ofs << name << '\t' << iter2->at( info->id_pos ).value.get() << std::endl;
          }
        }
      }
    }
  }
}


/*!
 *  \brief  �ޥ���ץ��å�����Ͽ����ID���դ��ؿ�
 *  \param[in]  line      ���ֹ�
 *  \param[in]  arg_list  �ޥ���°����ꥹ��
 *  \param[in]  p_ctx     �ޥ�����ƥ�����
 *  \retval     �ޥ����ֵ���
 */
toppers::macro_processor::var_t bf_assignid( toppers::text_line const& line,
                                             std::vector< toppers::macro_processor::var_t > const& arg_list,
                                             toppers::macro_processor::context* p_ctx )
{
  using namespace toppers;
  using namespace toppers::itronx;
  using toppers::text_line;
  typedef toppers::macro_processor::element element;
  typedef toppers::macro_processor::var_t var_t;
  typedef toppers::macro_processor::context context;

  return var_t();
}

//! �Ȥ߹��ߴؿ�ASSIGNID����Ͽ����
void register_bf_assignid( toppers::macro_processor* mproc )
{
  // �� �ɲ��Ȥ߹��ߴؿ�����Ͽ
  toppers::macro_processor::func_t func_info = {};
  func_info.name = "ASSIGNID";
  func_info.f = &bf_assignid;
  mproc->add_builtin_function( func_info );
}

/*!
 *  \brief  �ѥ�������
 *  \retval true  ����
 *  \retval false ����
 */
bool cfg2_main()
{
  using namespace toppers;
  using namespace toppers::itronx;

  std::string kernel( get_global< std::string >( "kernel" ) );
  itronx::factory factory( kernel );
  global( "factory" ) = &factory;

  // *.cfg��cfg1_out.srec���ɤ߹���
  std::string input_file;
  try
  {
    input_file = get_global< std::string >( "input-file" );
  }
  catch ( boost::bad_any_cast& )
  {
    fatal( _( "no input files" ) );
  }
  std::string cfg1_out_name( get_global< std::string >( "cfg1_out" ) );
  std::auto_ptr< cfg1_out > cfg1_out( factory.create_cfg1_out( cfg1_out_name ) );

  codeset_t codeset = get_global< codeset_t >( "codeset" );
  cfg1_out->load_cfg( input_file, codeset, *factory.get_static_api_info_map() );
  cfg1_out->load_srec();

  std::auto_ptr< macro_processor > mproc;
  std::auto_ptr< component > component_ptr;

  if ( get_global< bool >( "with-software-components" ) )
  {
    mproc = factory.create_macro_processor( *cfg1_out, cfg1_out->get_static_api_array() );
    component_ptr.reset( new component( mproc.get() ) );
  }
  else  // ������͡ʥ��եȥ������������б���
  {
    cfg1_out::static_api_map api_map( cfg1_out->merge() );
    assign_id( api_map ); // ID�ֹ�γ��դ�
    mproc = factory.create_macro_processor( *cfg1_out, api_map );
  }

  // �ƥ�ץ졼�Ƚ���
  boost::any template_file( global( "template-file" ) );
  namespace fs = boost::filesystem;
//  fs::path cfg_dir( get_global< std::string >( "cfg-directory" ), fs::native );
  fs::path cfg_dir( get_global< std::string >( "cfg-directory" ) );  // filesystem3�б�
  std::vector< std::string > include_paths = get_global< std::vector< std::string > >( "include-path" );
//  include_paths.push_back( cfg_dir.empty() ? "." : cfg_dir.native_file_string() );
  include_paths.push_back( cfg_dir.empty() ? "." : cfg_dir.string() );  // filesystem3�б�
  if ( !template_file.empty() )
  {
    toppers::text in_text;
    toppers::text pp_text;
    std::string file_name( boost::any_cast< std::string& >( template_file ) );

    in_text.set_line( file_name, 1 );
    std::ifstream ifs( file_name.c_str() );
    in_text.append( ifs );
    macro_processor::preprocess( in_text, pp_text );
    mproc->evaluate( pp_text );
  }
  else  // �ƥ�ץ졼�ȥե����뤬���ꤵ��Ƥ��ʤ��Τǡ�������ʬ��kernel.tf�ˤΤߤ����
  {
    fs::path kernel_cfg_template_file( cfg_dir/fs::path( "../../kernel/kernel.tf" ) );
    if ( !fs::exists( kernel_cfg_template_file ) )
    {
//      error( _( "cannot open file `%1%\'" ), kernel_cfg_template_file.native_file_string() );
      error( _( "cannot open file `%1%\'" ), kernel_cfg_template_file.string() );  // filesystem3�б�
    }
    else
    {
      toppers::text in_text;
      toppers::text pp_text;

//      in_text.set_line( kernel_cfg_template_file.native_file_string(), 1 );
      in_text.set_line( kernel_cfg_template_file.string(), 1 );  // filesystem3�б�
//      std::ifstream ifs( kernel_cfg_template_file.native_file_string().c_str() );
      std::ifstream ifs( kernel_cfg_template_file.string().c_str() );  // filesystem3�б�
      in_text.append( ifs );
      macro_processor::preprocess( in_text, pp_text );
      mproc->evaluate( pp_text );
    }
  }

  if ( get_error_count() > 0 )
  {
    return false;
  }
  output_file::save();
  return true;
}
