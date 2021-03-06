/*
* This file is part of BRAT
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef __BRAT_TASK_H__
#define __BRAT_TASK_H__

#include <string>

#include <QDateTime>

#include "new-gui/Common/QtStringUtils.h"
#include "new-gui/Common/tools/Trace.h"

#include "libbrathl/brathl.h" 
#include "libbrathl/BratAlgorithmBase.h" 


using namespace brathl;


class CBratTask;



//-------------------------------------------------------------
//------------------- CVectorBratTask class --------------------
//-------------------------------------------------------------

class CVectorBratTask : public std::vector<CBratTask*>
{
	typedef std::vector<CBratTask*> base_t;

	bool m_bDelete;

public:
	CVectorBratTask( bool bDelete = true ) 
		: base_t()
		, m_bDelete( bDelete )
	{}

	virtual ~CVectorBratTask()
	{
		RemoveAll();
	}

	bool operator == ( const CVectorBratTask &o ) const;		//v4: new

    bool GetDelete() { return m_bDelete; }
    void SetDelete( bool value ) { m_bDelete = value; }

	virtual void Insert( CBratTask* ob )
	{
		push_back( ob );
	}

	virtual void Insert( const CVectorBratTask* vec, bool bRemoveAll = true );

	virtual CVectorBratTask::iterator InsertAt( CVectorBratTask::iterator where, CBratTask* ob )
	{
		return insert( where, ob );
	}

	virtual void RemoveAll();

	virtual void Dump( std::ostream& fOut = std::cerr );
};

//-------------------------------------------------------------
//------------------- CBratTaskFunction class --------------------
//-------------------------------------------------------------

typedef void (*BratTaskFunctionCallableN)(CVectorBratAlgorithmParam& arg);

class CBratTaskFunction
{
public:
	static const std::string sm_TASK_FUNC_COPYFILE;

	static void BratTaskFunctionCopyFile( CVectorBratAlgorithmParam& arg );		//v3 called CopyFile; name clash with window API

protected:
	std::string m_name;
    BratTaskFunctionCallableN m_call = nullptr;
	CVectorBratAlgorithmParam m_params;

public:
	CBratTaskFunction()
	{}
    CBratTaskFunction( const std::string& name, BratTaskFunctionCallableN call ) :
		m_name( name ), m_call( call )
	{}
	CBratTaskFunction( const CBratTaskFunction& o )
	{
		*this = o;
	}
	const CBratTaskFunction& operator=( const CBratTaskFunction& o )
	{
		if ( this != &o )
		{
			m_name = o.m_name;
			m_call = o.m_call;
			//m_params = o.m_params;	//v4: v3 ignored m_params; maybe because CBratTaskFunction instances are also used in the global functions map, where they are sort of "abstract"; nice
		}
		return *this;
	}

	virtual ~CBratTaskFunction()
	{}

	bool operator == ( const CBratTaskFunction& o ) const
	{
		return 
			m_name == o.m_name
			&& m_call == o.m_call
			//&& m_params == o.m_params	//v4: follow assignment operator and ignore m_params in equality test
			;
	}

    const std::string& GetName() const { return m_name; }
    void SetName( const std::string& value ) { m_name = value; }

    BratTaskFunctionCallableN GetCall() { return m_call; }
    void SetCall( BratTaskFunctionCallableN value ) { m_call = value; }

    const CVectorBratAlgorithmParam* GetParams() const { return &m_params; }
    CVectorBratAlgorithmParam* GetParams() { return &m_params; }
	void GetParamsAsString( std::string& value ) const
	{
	  value = m_params.ToString().c_str();
	}

	void Execute();

	virtual void Dump( std::ostream& fOut = std::cerr );
};


//-------------------------------------------------------------
//------------------- CMapBratTaskFunction class --------------------
//-------------------------------------------------------------
typedef std::map<std::string, CBratTaskFunction*> mapbrattaskfunction; 

class CMapBratTaskFunction: public mapbrattaskfunction
{
protected:
  CMapBratTaskFunction(bool bDelete = true);

public:

  virtual ~CMapBratTaskFunction();

  static CMapBratTaskFunction&  GetInstance();

  virtual CBratTaskFunction* Insert(const std::string& key, CBratTaskFunction* ob, bool withExcept = true);

  virtual bool Remove(const std::string& id);
  virtual bool Remove(CMapBratTaskFunction::iterator it);

  virtual void RemoveAll();

  virtual void Dump(std::ostream& fOut = std::cerr); 

  virtual CBratTaskFunction* Find(const std::string& id) const;

  bool GetDelete() {return m_bDelete;}
  void SetDelete(bool value) {m_bDelete = value;}

protected:

  void Init();

protected:

  bool m_bDelete;



};
//-------------------------------------------------------------
//------------------- CBratTask class --------------------
//-------------------------------------------------------------


class task;


class CBratTask
{
	//types

public:

	typedef long long uid_t;

	enum EStatus
	{
		eBRAT_STATUS_PENDING,
		eBRAT_STATUS_PROCESSING,
		eBRAT_STATUS_ENDED,
		eBRAT_STATUS_ERROR,
		eBRAT_STATUS_WARNING,

		eStatus_size
	};

	//static members (functions and data)

protected:
	static const std::string* statusNames()		//XSD: these are values of the XSD enumeration of status attribute
	{
		static const std::string labels[ eStatus_size ] =
		{
			"pending",
			"in progress",
			"ended",
			"error",
			"warning"
		};

		static_assert( ARRAY_SIZE( labels ) == eStatus_size, "CBratTask: Number of status labels and values does not match." );

		return labels;
	}


public:
	static const std::string& TaskStatusToString( EStatus s )
	{
		assert__( s < eStatus_size );

		return statusNames()[ s ];
	}

	static EStatus StringToTaskStatus( const std::string& status )
	{
		static const std::string *labels = statusNames();

		auto it = std::find( labels, &labels[ eStatus_size ], status );
		if ( it != &labels[ eStatus_size ] )
			return static_cast<EStatus>( it - labels );

		throw CException( "CBratTask::StringToTaskStatus: unknown status label " + status, BRATHL_INCONSISTENCY_ERROR );
	}


public:

	// datetime in ISO 8601 format
	static const std::string& formatISODateTime()
	{
		static const std::string FormatISODateTime = "%Y-%m-%d %H:%M:%S";
		return FormatISODateTime;
	}

protected:
	static const QString& qformatISODateTime()
	{
		static const QString FormatISODateTime = "yyyy-MM-dd hh:mm:ss";
		return FormatISODateTime;
	}

	//instance data members
	
protected:
	uid_t m_uid = -1;										//1
	std::string m_name;										//2
	std::string m_cmd;										//3
	CBratTaskFunction m_function;							//4
	QDateTime m_at;											//5	//typedef wxDateTime QDateTime;
	EStatus m_status = CBratTask::eBRAT_STATUS_PENDING;		//6
	std::string m_logFile;									//7

	CVectorBratTask m_subordinateTasks;						//8

public:
	// construction / destruction

	CBratTask()
	{}

	CBratTask( uid_t uid, const std::string &name, const std::string &cmd, const QDateTime &at, EStatus status, const std::string &log_file )
		: m_uid( uid )
		, m_name( name )
		, m_cmd( cmd )
		, m_at( at )
		, m_status( status )
		, m_logFile( log_file )
	{}

	CBratTask( uid_t uid, const std::string &name, const CBratTaskFunction &function, const QDateTime &at, EStatus status, const std::string &log_file )
		: m_uid( uid )
		, m_name( name )
        , m_function( function )
		, m_at( at )
		, m_status( status )
		, m_logFile( log_file )
	{}

	CBratTask( const CBratTask &o )
	{
		*this = o;
	}

	const CBratTask& operator=( const CBratTask &o )
	{
		if ( this != &o )
		{
			m_uid = o.m_uid;				//1
			m_name = o.m_name;				//2
			m_cmd = o.m_cmd;				//3
			m_function = o.m_function;		//4
			m_at = o.m_at;					//5
			m_status = o.m_status;			//6
			m_logFile = o.m_logFile;		//7

			m_subordinateTasks.Insert( &o.m_subordinateTasks );		//8		//CHECK THIS !!! Apparently only called here
		}
		return *this;
	}

	virtual CBratTask* Clone() { return new CBratTask( *this ); }

	virtual ~CBratTask()
	{}

	// serialization

	CBratTask( const task &oxml );				//USED ON LOAD

	task& IOcopy( task &oxml ) const;			//USED ON STORE

	// equality

	bool operator == ( const CBratTask &o ) const 
	{
		return 
			m_uid == o.m_uid &&							//1
			m_name == o.m_name &&						//2
			m_cmd == o.m_cmd &&							//3
			m_function == o.m_function &&				//4
			m_at == o.m_at &&							//5
			m_status == o.m_status &&					//6
			m_logFile == o.m_logFile &&					//7
			m_subordinateTasks == o.m_subordinateTasks	//8
			;
	}
	bool operator != ( const CBratTask& o ) const
	{
		return !( *this == o );
	}

	// getters / setters

	const uid_t GetUid() const { return m_uid; }					//uid_t GetUidValue() const { return m_uid; }
	std::string GetUidAsString() const { return n2s( m_uid ); }
	void SetUid( const std::string& value )
	{
		m_uid = s2n<uid_t>( value );	//v4 m_uid = wxBratTools::wxStringTowxLongLong_t(value);
	}
	void SetUid( uid_t value ) { m_uid = value; }

	//void SetId(const std::string& value) {m_id = value;};
	//const std::string& GetId() {return m_id;};

	const std::string& GetName() const { return m_name; }
	void SetName( const std::string& value ) { m_name = value; }

	const QDateTime& GetAt() const { return m_at; }
	std::string GetAtAsString( /*const std::string& format = formatISODateTime() femm*/) const 
	{ 
		QString s = m_at.toString( qformatISODateTime() );
		
		// "If the format is Qt::ISODate, the string format corresponds to the ISO 8601 extended 
		//	specification for representations of dates and times, taking the form YYYY-MM-DDTHH:mm:ss[Z|[+|-]HH:mm], 
		//	depending on the timeSpec() of the QDateTime. If the timeSpec() is Qt::UTC, Z will 
		//	be appended to the string; if the timeSpec() is Qt::OffsetFromUTC the offset in hours 
		//	and minutes from UTC will be appended to the string."

		return q2t<std::string>( s );
	}
	//void SetAt( const QDateTime& value ) { m_at = value; }
	void SetAt( const std::string &value )
	{ 
		QDateTime dt = QDateTime::fromString( t2q( value ), qformatISODateTime() );
		assert__( dt.isValid() );															 	//!!! TODO !!!: error handling
		m_at = dt; 
	}
	bool laterThanNow() const { return GetAt() > QDateTime::currentDateTime(); }

	const std::string& GetCmd() const { return m_cmd; }
	void SetCmd( const std::string &value ) { m_cmd = value; }

	EStatus GetStatus() const { return m_status; }
	std::string GetStatusAsString() const { return TaskStatusToString( m_status ); }
	void SetStatus( const std::string& value ) { m_status = CBratTask::StringToTaskStatus( value ); }
	void SetStatus( EStatus value ) { m_status = value; }

	const std::string& GetLogFile() const { return m_logFile; }				//std::string GetLogFileAsString() { return m_logFile.GetFullPath(); }	
	void SetLogFile( const std::string& value ) { m_logFile = value; }		//void SetLogFile( const std::string& value ) { m_logFile.Assign( value ); }

	CVectorBratTask* GetSubordinateTasks() { return &m_subordinateTasks; }
	bool HasSubordinateTasks() { return ( m_subordinateTasks.size() > 0 ); }

	const CBratTaskFunction* GetBratTaskFunction() const { return &m_function; }
	CBratTaskFunction* GetBratTaskFunction() { return &m_function; }
	void SetBratTaskFunction( const CBratTaskFunction& value ) { m_function = value; }
	void SetBratTaskFunction( CBratTaskFunction* value )
	{
		if ( value != NULL )
		{
			m_function = *value;
		}
	};

	// testers

	bool HasCmd() { return !( m_cmd.empty() ); }
	bool HasFunction() { return !( m_function.GetName().empty() ); }

	// remaining data members

	void ExecuteFunction()
	{
		m_function.Execute();
	}

	//v4
	//static std::string TaskStatusToString( EStatus status );
	//static EStatus StringToTaskStatus( const std::string& status );

	// Dump function
	virtual void Dump( std::ostream& fOut = std::cerr );
};



//-------------------------------------------------------------
//------------------- CMapBratTask class --------------------
//-------------------------------------------------------------


class CMapBratTask : public std::map< CBratTask::uid_t, CBratTask* >
{
	//types

	typedef std::map< CBratTask::uid_t, CBratTask* > base_t; 
	typedef CBratTask::uid_t uid_t;

	//data members

	bool m_bDelete;

	// constructors / destructor

public:
	CMapBratTask( bool bDelete = true ) : base_t(), m_bDelete( bDelete )
	{}
	virtual ~CMapBratTask()
	{
		RemoveAll();
	}

	// getters / setters & access

    bool GetDelete() const { return m_bDelete; }
    void SetDelete( bool value ) { m_bDelete = value; }

	virtual CBratTask* Find( uid_t id ) const
	{
		const_iterator it = find( id );
		if ( it == end() )
			return NULL;

		return it->second;
	}

	// insert

	//If 'key' already exists --> pairInsert.second == false and
	// pairInsert.first then contains an iterator on the existing object
	// If 'key' does not exist --> pairInsert.second == true and
	// pairInsert.first then contains a iterator on the inserted object
	//
	virtual CBratTask* Insert( uid_t key, CBratTask* ob, bool withExcept = true )
	{
		std::pair <iterator, bool> pairInsert = insert( value_type( key, ob ) );

		if ( !pairInsert.second && withExcept )
		{
			CException e( "ERROR in CMapBratTask::Insert - try to insert an task that already exists. Check that no task have the same id", BRATHL_INCONSISTENCY_ERROR );
			Dump(*CTrace::GetDumpContext());
			throw e;
		}

		CMapBratTask::iterator it = ( pairInsert.first );
		return it->second;
	}

	virtual void Insert( const CMapBratTask* map, bool bRemoveAll = true )
	{
		if ( map == NULL )
			return;

		if ( bRemoveAll )
			RemoveAll();

		for ( const_iterator it = map->begin(); it != map->end(); it++ )
			Insert( it->first, it->second->Clone() );
	}

	// remove

	virtual bool Remove( uid_t id )
	{
		return Remove( find( id ) );
	}

	virtual bool Remove( iterator it )
	{
		if ( it == end() )
			return false;

		if ( m_bDelete && it->second != NULL )
			delete it->second;

		base_t::erase( it );

		return true;
	}

	virtual void RemoveAll()
	{
		if ( m_bDelete )
            DestroyPointersAndContainer( *this );
		else
			clear();
	}

	// ...

	virtual void Dump( std::ostream& fOut = std::cerr );
};


#endif		// __BRAT_TASK_H__
