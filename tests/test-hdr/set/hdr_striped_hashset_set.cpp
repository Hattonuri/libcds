/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     
*/

#include "set/hdr_striped_set.h"
#include <cds/container/striped_set/std_set.h>
#include <cds/container/striped_set.h>
#include <cds/sync/spinlock.h>

namespace set {

    namespace {
        struct my_copy_policy {
            typedef std::set<StripedSetHdrTest::item, StripedSetHdrTest::less<StripedSetHdrTest::item> > set_type;
            typedef set_type::iterator iterator;

            void operator()( set_type& set, iterator itWhat )
            {
                set.insert( std::make_pair(itWhat->key(), itWhat->val()) );
            }
        };

        typedef std::set<StripedSetHdrTest::item, StripedSetHdrTest::less<StripedSetHdrTest::item> > set_t;
    }

    void StripedSetHdrTest::Striped_set()
    {
        CPPUNIT_MESSAGE( "cmp");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            ,co::mutex_policy< cc::striped_set::striping<> >
        >   set_cmp;
        test_striped< set_cmp >();

        CPPUNIT_MESSAGE( "less");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::less< less<item> >
            ,co::mutex_policy< cc::striped_set::striping<> >
        >   set_less;
        test_striped< set_less >();

        CPPUNIT_MESSAGE( "cmpmix");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            , co::less< less<item> >
            ,co::mutex_policy< cc::striped_set::striping<> >
        >   set_cmpmix;
        test_striped< set_cmpmix >();

        // Spinlock as lock policy
        CPPUNIT_MESSAGE( "spinlock");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::less< less<item> >
            , co::mutex_policy< cc::striped_set::striping< cds::sync::spin> >
        >   set_spin;
        test_striped< set_spin >();

        // Resizing policy
        CPPUNIT_MESSAGE( "load_factor_resizing<0>(1024)");
        {
            typedef cc::StripedSet< set_t
                , co::hash< hash_int >
                , co::less< less<item> >
                , co::resizing_policy< cc::striped_set::load_factor_resizing<0> >
            >   set_less_resizing_lf;
            set_less_resizing_lf s(30, cc::striped_set::load_factor_resizing<0>( 1024 ));
            test_striped_with( s );
        }

        CPPUNIT_MESSAGE( "load_factor_resizing<256>");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::less< less<item> >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<256> >
        >   set_less_resizing_lf16;
        test_striped< set_less_resizing_lf16 >();

        CPPUNIT_MESSAGE( "single_bucket_size_threshold<0>(1024");
        {
            typedef cc::StripedSet< set_t
                , co::hash< hash_int >
                , co::less< less<item> >
                , co::resizing_policy< cc::striped_set::single_bucket_size_threshold<0> >
            >   set_less_resizing_sbt;
            set_less_resizing_sbt s( 30, cc::striped_set::single_bucket_size_threshold<0>(1024));
            test_striped_with(s);
        }

        CPPUNIT_MESSAGE( "single_bucket_size_threshold<256>");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::less< less<item> >
            , co::resizing_policy< cc::striped_set::single_bucket_size_threshold<256> >
        >   set_less_resizing_sbt16;
        test_striped< set_less_resizing_sbt16 >();

        // Copy policy
        CPPUNIT_MESSAGE( "load_factor_resizing<256>, copy_item");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<256> >
            , co::copy_policy< cc::striped_set::copy_item >
        >   set_copy_item;
        test_striped< set_copy_item >();

        CPPUNIT_MESSAGE( "load_factor_resizing<256>, swap_item");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<256> >
            , co::copy_policy< cc::striped_set::swap_item >
        >   set_swap_item;
        test_striped< set_swap_item >();

        CPPUNIT_MESSAGE( "load_factor_resizing<256>, move_item");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<256> >
            , co::copy_policy< cc::striped_set::move_item >
        >   set_move_item;
        test_striped< set_move_item >();

        CPPUNIT_MESSAGE( "load_factor_resizing<256>, special copy_item");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<256> >
            , co::copy_policy< my_copy_policy >
        >   set_special_copy_item;
        test_striped< set_special_copy_item >();
    }

}   // namespace set
