//
// Created by igor on 16/07/2020.
//


#include <doctest.h>
#include <string>
#include <string.h>
#include "plugin/pipeline/csp/base64.hh"


namespace
{
    std::string s_tiny = "Hey";
    std::string s_tiny_expected = "SGV5";

    std::string s_short = "Hello World!!!";
    std::string s_short_expected = "SGVsbG8gV29ybGQhISE=";

    std::string s_medium = R"(Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam ex nunc, rhoncus in blandit at, rutrum sed turpis. Aenean in bibendum dolor, vitae facilisis dolor. Quisque imperdiet et nulla non feugiat. Fusce elementum est eu nibh efficitur aliquet. Quisque elementum diam libero, eget auctor nunc condimentum in.)";
    std::string s_medium_expected = R"(TG9yZW0gaXBzdW0gZG9sb3Igc2l0IGFtZXQsIGNvbnNlY3RldHVyIGFkaXBpc2NpbmcgZWxpdC4gRXRpYW0gZXggbnVuYywgcmhvbmN1cyBpbiBibGFuZGl0IGF0LCBydXRydW0gc2VkIHR1cnBpcy4gQWVuZWFuIGluIGJpYmVuZHVtIGRvbG9yLCB2aXRhZSBmYWNpbGlzaXMgZG9sb3IuIFF1aXNxdWUgaW1wZXJkaWV0IGV0IG51bGxhIG5vbiBmZXVnaWF0LiBGdXNjZSBlbGVtZW50dW0gZXN0IGV1IG5pYmggZWZmaWNpdHVyIGFsaXF1ZXQuIFF1aXNxdWUgZWxlbWVudHVtIGRpYW0gbGliZXJvLCBlZ2V0IGF1Y3RvciBudW5jIGNvbmRpbWVudHVtIGluLg==)";

    std::string s_long = R"(Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam ex nunc, rhoncus in blandit at, rutrum sed turpis. Aenean in bibendum dolor, vitae facilisis dolor. Quisque imperdiet et nulla non feugiat. Fusce elementum est eu nibh efficitur aliquet. Quisque elementum diam libero, eget auctor nunc condimentum in. Maecenas sit amet est maximus, ultricies quam ac, sagittis odio. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Vestibulum pharetra nisl non lacus pharetra lobortis. Vivamus vel tortor ac nulla lobortis scelerisque in sit amet enim.
Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer vitae erat accumsan quam eleifend suscipit. Nullam finibus mi a lacus lobortis vehicula. Nulla facilisi. Quisque molestie odio et nulla scelerisque, in maximus turpis posuere. Suspendisse magna quam, imperdiet ac diam eget, pharetra fringilla eros. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Proin ac ex sapien. Praesent accumsan consequat sapien, vel pretium ligula hendrerit in. Aenean sit amet ligula nec leo tincidunt egestas. Nulla ullamcorper eros sed risus venenatis, ut eleifend nunc vulputate. Praesent egestas nisi sed quam gravida, a tempor sem interdum. Nam a arcu nibh.
Integer vulputate nunc ac erat sollicitudin porta id id risus. Fusce nec aliquam risus, at tincidunt sapien. Suspendisse dignissim, erat eu rutrum mollis, lacus lorem lacinia enim, nec ornare nibh justo commodo nibh. Proin velit mi, dapibus non tincidunt id, pretium in metus. Morbi eu rhoncus tortor, ac tempor lacus. In vehicula non purus eu faucibus. Vestibulum volutpat nunc id pharetra posuere. Sed sagittis ligula in fermentum viverra. Aliquam aliquet sed nisi non sodales. Duis ullamcorper urna quam, in imperdiet urna ullamcorper quis. In hac habitasse platea dictumst.
Donec cursus, felis a pulvinar tempus, elit mi hendrerit felis, et volutpat eros felis sed lacus. Nulla est ipsum, molestie sed nisl in, sagittis laoreet ex. Curabitur dapibus egestas lobortis. Morbi pulvinar placerat tellus sed sollicitudin. Ut finibus suscipit mi, at fermentum purus tincidunt eu. Nulla suscipit magna vel massa tincidunt laoreet at at odio. Fusce nec magna eros.
Interdum et malesuada fames ac ante ipsum primis in faucibus. Nam condimentum, velit dictum scelerisque tincidunt, arcu libero fermentum libero, lacinia hendrerit lorem tellus quis lectus. Donec placerat sodales eros rutrum tincidunt. Aliquam erat volutpat. Interdum et malesuada fames ac ante ipsum primis in faucibus. Praesent id nulla at justo rutrum ultrices. Vestibulum hendrerit elit eget pharetra porta. Nunc euismod velit nulla, a faucibus purus venenatis sed. Donec id augue lobortis purus vestibulum fermentum. Suspendisse nibh nisi, malesuada in pulvinar et, pretium blandit turpis. Interdum et malesuada fames ac ante ipsum primis in faucibus. Etiam at mollis ex. Cras in nunc quis lacus aliquam laoreet. Ut tincidunt nisl ac sapien dictum, id suscipit elit mattis. Pellentesque tristique interdum lobortis.)";
    std::string s_long_expected = R"(TG9yZW0gaXBzdW0gZG9sb3Igc2l0IGFtZXQsIGNvbnNlY3RldHVyIGFkaXBpc2NpbmcgZWxpdC4gRXRpYW0gZXggbnVuYywgcmhvbmN1cyBpbiBibGFuZGl0IGF0LCBydXRydW0gc2VkIHR1cnBpcy4gQWVuZWFuIGluIGJpYmVuZHVtIGRvbG9yLCB2aXRhZSBmYWNpbGlzaXMgZG9sb3IuIFF1aXNxdWUgaW1wZXJkaWV0IGV0IG51bGxhIG5vbiBmZXVnaWF0LiBGdXNjZSBlbGVtZW50dW0gZXN0IGV1IG5pYmggZWZmaWNpdHVyIGFsaXF1ZXQuIFF1aXNxdWUgZWxlbWVudHVtIGRpYW0gbGliZXJvLCBlZ2V0IGF1Y3RvciBudW5jIGNvbmRpbWVudHVtIGluLiBNYWVjZW5hcyBzaXQgYW1ldCBlc3QgbWF4aW11cywgdWx0cmljaWVzIHF1YW0gYWMsIHNhZ2l0dGlzIG9kaW8uIFBlbGxlbnRlc3F1ZSBoYWJpdGFudCBtb3JiaSB0cmlzdGlxdWUgc2VuZWN0dXMgZXQgbmV0dXMgZXQgbWFsZXN1YWRhIGZhbWVzIGFjIHR1cnBpcyBlZ2VzdGFzLiBWZXN0aWJ1bHVtIHBoYXJldHJhIG5pc2wgbm9uIGxhY3VzIHBoYXJldHJhIGxvYm9ydGlzLiBWaXZhbXVzIHZlbCB0b3J0b3IgYWMgbnVsbGEgbG9ib3J0aXMgc2NlbGVyaXNxdWUgaW4gc2l0IGFtZXQgZW5pbS4KTG9yZW0gaXBzdW0gZG9sb3Igc2l0IGFtZXQsIGNvbnNlY3RldHVyIGFkaXBpc2NpbmcgZWxpdC4gSW50ZWdlciB2aXRhZSBlcmF0IGFjY3Vtc2FuIHF1YW0gZWxlaWZlbmQgc3VzY2lwaXQuIE51bGxhbSBmaW5pYnVzIG1pIGEgbGFjdXMgbG9ib3J0aXMgdmVoaWN1bGEuIE51bGxhIGZhY2lsaXNpLiBRdWlzcXVlIG1vbGVzdGllIG9kaW8gZXQgbnVsbGEgc2NlbGVyaXNxdWUsIGluIG1heGltdXMgdHVycGlzIHBvc3VlcmUuIFN1c3BlbmRpc3NlIG1hZ25hIHF1YW0sIGltcGVyZGlldCBhYyBkaWFtIGVnZXQsIHBoYXJldHJhIGZyaW5naWxsYSBlcm9zLiBMb3JlbSBpcHN1bSBkb2xvciBzaXQgYW1ldCwgY29uc2VjdGV0dXIgYWRpcGlzY2luZyBlbGl0LiBQcm9pbiBhYyBleCBzYXBpZW4uIFByYWVzZW50IGFjY3Vtc2FuIGNvbnNlcXVhdCBzYXBpZW4sIHZlbCBwcmV0aXVtIGxpZ3VsYSBoZW5kcmVyaXQgaW4uIEFlbmVhbiBzaXQgYW1ldCBsaWd1bGEgbmVjIGxlbyB0aW5jaWR1bnQgZWdlc3Rhcy4gTnVsbGEgdWxsYW1jb3JwZXIgZXJvcyBzZWQgcmlzdXMgdmVuZW5hdGlzLCB1dCBlbGVpZmVuZCBudW5jIHZ1bHB1dGF0ZS4gUHJhZXNlbnQgZWdlc3RhcyBuaXNpIHNlZCBxdWFtIGdyYXZpZGEsIGEgdGVtcG9yIHNlbSBpbnRlcmR1bS4gTmFtIGEgYXJjdSBuaWJoLgpJbnRlZ2VyIHZ1bHB1dGF0ZSBudW5jIGFjIGVyYXQgc29sbGljaXR1ZGluIHBvcnRhIGlkIGlkIHJpc3VzLiBGdXNjZSBuZWMgYWxpcXVhbSByaXN1cywgYXQgdGluY2lkdW50IHNhcGllbi4gU3VzcGVuZGlzc2UgZGlnbmlzc2ltLCBlcmF0IGV1IHJ1dHJ1bSBtb2xsaXMsIGxhY3VzIGxvcmVtIGxhY2luaWEgZW5pbSwgbmVjIG9ybmFyZSBuaWJoIGp1c3RvIGNvbW1vZG8gbmliaC4gUHJvaW4gdmVsaXQgbWksIGRhcGlidXMgbm9uIHRpbmNpZHVudCBpZCwgcHJldGl1bSBpbiBtZXR1cy4gTW9yYmkgZXUgcmhvbmN1cyB0b3J0b3IsIGFjIHRlbXBvciBsYWN1cy4gSW4gdmVoaWN1bGEgbm9uIHB1cnVzIGV1IGZhdWNpYnVzLiBWZXN0aWJ1bHVtIHZvbHV0cGF0IG51bmMgaWQgcGhhcmV0cmEgcG9zdWVyZS4gU2VkIHNhZ2l0dGlzIGxpZ3VsYSBpbiBmZXJtZW50dW0gdml2ZXJyYS4gQWxpcXVhbSBhbGlxdWV0IHNlZCBuaXNpIG5vbiBzb2RhbGVzLiBEdWlzIHVsbGFtY29ycGVyIHVybmEgcXVhbSwgaW4gaW1wZXJkaWV0IHVybmEgdWxsYW1jb3JwZXIgcXVpcy4gSW4gaGFjIGhhYml0YXNzZSBwbGF0ZWEgZGljdHVtc3QuCkRvbmVjIGN1cnN1cywgZmVsaXMgYSBwdWx2aW5hciB0ZW1wdXMsIGVsaXQgbWkgaGVuZHJlcml0IGZlbGlzLCBldCB2b2x1dHBhdCBlcm9zIGZlbGlzIHNlZCBsYWN1cy4gTnVsbGEgZXN0IGlwc3VtLCBtb2xlc3RpZSBzZWQgbmlzbCBpbiwgc2FnaXR0aXMgbGFvcmVldCBleC4gQ3VyYWJpdHVyIGRhcGlidXMgZWdlc3RhcyBsb2JvcnRpcy4gTW9yYmkgcHVsdmluYXIgcGxhY2VyYXQgdGVsbHVzIHNlZCBzb2xsaWNpdHVkaW4uIFV0IGZpbmlidXMgc3VzY2lwaXQgbWksIGF0IGZlcm1lbnR1bSBwdXJ1cyB0aW5jaWR1bnQgZXUuIE51bGxhIHN1c2NpcGl0IG1hZ25hIHZlbCBtYXNzYSB0aW5jaWR1bnQgbGFvcmVldCBhdCBhdCBvZGlvLiBGdXNjZSBuZWMgbWFnbmEgZXJvcy4KSW50ZXJkdW0gZXQgbWFsZXN1YWRhIGZhbWVzIGFjIGFudGUgaXBzdW0gcHJpbWlzIGluIGZhdWNpYnVzLiBOYW0gY29uZGltZW50dW0sIHZlbGl0IGRpY3R1bSBzY2VsZXJpc3F1ZSB0aW5jaWR1bnQsIGFyY3UgbGliZXJvIGZlcm1lbnR1bSBsaWJlcm8sIGxhY2luaWEgaGVuZHJlcml0IGxvcmVtIHRlbGx1cyBxdWlzIGxlY3R1cy4gRG9uZWMgcGxhY2VyYXQgc29kYWxlcyBlcm9zIHJ1dHJ1bSB0aW5jaWR1bnQuIEFsaXF1YW0gZXJhdCB2b2x1dHBhdC4gSW50ZXJkdW0gZXQgbWFsZXN1YWRhIGZhbWVzIGFjIGFudGUgaXBzdW0gcHJpbWlzIGluIGZhdWNpYnVzLiBQcmFlc2VudCBpZCBudWxsYSBhdCBqdXN0byBydXRydW0gdWx0cmljZXMuIFZlc3RpYnVsdW0gaGVuZHJlcml0IGVsaXQgZWdldCBwaGFyZXRyYSBwb3J0YS4gTnVuYyBldWlzbW9kIHZlbGl0IG51bGxhLCBhIGZhdWNpYnVzIHB1cnVzIHZlbmVuYXRpcyBzZWQuIERvbmVjIGlkIGF1Z3VlIGxvYm9ydGlzIHB1cnVzIHZlc3RpYnVsdW0gZmVybWVudHVtLiBTdXNwZW5kaXNzZSBuaWJoIG5pc2ksIG1hbGVzdWFkYSBpbiBwdWx2aW5hciBldCwgcHJldGl1bSBibGFuZGl0IHR1cnBpcy4gSW50ZXJkdW0gZXQgbWFsZXN1YWRhIGZhbWVzIGFjIGFudGUgaXBzdW0gcHJpbWlzIGluIGZhdWNpYnVzLiBFdGlhbSBhdCBtb2xsaXMgZXguIENyYXMgaW4gbnVuYyBxdWlzIGxhY3VzIGFsaXF1YW0gbGFvcmVldC4gVXQgdGluY2lkdW50IG5pc2wgYWMgc2FwaWVuIGRpY3R1bSwgaWQgc3VzY2lwaXQgZWxpdCBtYXR0aXMuIFBlbGxlbnRlc3F1ZSB0cmlzdGlxdWUgaW50ZXJkdW0gbG9ib3J0aXMu)";
}

TEST_SUITE("base64") {
    TEST_CASE("test short") {
        std::string out = base64::encode((base64::byte *)s_short.data(), s_short.size());
        REQUIRE(out == s_short_expected);
        std::vector<base64::byte> dec = base64::decode(out);
        REQUIRE(s_short.size() == dec.size());
        REQUIRE(memcmp(s_short.c_str(), dec.data(), dec.size()) == 0);
    }
    TEST_CASE("test tiny") {
        std::string out = base64::encode((base64::byte *)s_tiny.data(), s_tiny.size());
        REQUIRE(out == s_tiny_expected);
        std::vector<base64::byte> dec = base64::decode(out);
        REQUIRE(s_tiny.size() == dec.size());
        REQUIRE(memcmp(s_tiny.c_str(), dec.data(), dec.size()) == 0);
    }
    TEST_CASE("test medium") {
        std::string out = base64::encode((base64::byte *)s_medium.data(), s_medium.size());
        REQUIRE(out == s_medium_expected);
        std::vector<base64::byte> dec = base64::decode(out);
        REQUIRE(s_medium.size() == dec.size());
        REQUIRE(memcmp(s_medium.c_str(), dec.data(), dec.size()) == 0);
    }

    TEST_CASE("test long") {
        std::string out = base64::encode((base64::byte *)s_long.data(), s_long.size());
        REQUIRE(out == s_long_expected);
        std::vector<base64::byte> dec = base64::decode(out);
        REQUIRE(s_long.size() == dec.size());
        REQUIRE(memcmp(s_long.c_str(), dec.data(), dec.size()) == 0);
    }
}
